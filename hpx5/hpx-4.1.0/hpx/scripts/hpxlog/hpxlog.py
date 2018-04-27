#!/usr/bin/env python

#Python tools for working with hpx log files
#To install run "pip install ." in the same directory as this file
#To use as a script, add this file to your script search path

from __future__ import print_function
import numpy as np
from numpy.lib.recfunctions import merge_arrays
import struct, ast, itertools



def save_numpy(target, log, compress=False):
    """Save log dictionary to numpy format

    compress -- Save with compression. Default False
    """
    fn = np.savez_compressed if compress else np.savez
    fn(target, **log)    



def load_as_numpy(filename, filter_pad=True):
    """
    Reads an HPX produced log file, returns a dictionary with numpy data and
    metadata, similar to loading from a .npz file with multiple entries.
    
    If the file does not start with "HPXnpy" (HPX numpy) an exception is raised.

    Options:
    filter_pad -- Remove "--pad" fields from output before returning. Default True.
    """
    fmt = "<BBH"
    with open(filename, "r") as f:
        magic = f.read(6)
        if magic != "HPXnpy": raise IOError(filename + " does not appear to be HPXnp formatted.")
        
        (major, minor, meta_len) = struct.unpack(fmt, f.read(struct.calcsize(fmt)))
        metadata = f.read(meta_len)
        meta = ast.literal_eval(metadata.strip('\x00'))
        data = np.fromfile(f, dtype=meta['descr'])
        
        if filter_pad:
            names = [n for n in data.dtype.names if not n.startswith("--pad")]
            data = data[names].copy()
        
        # Consts dict to numpy record array
        meta = meta.get("consts", {})
        vals = [e[0] for e in meta.values()]
        types = [e[1] for e in meta.values()]        
        consts_arr = np.array([tuple(vals)], dtype=zip(meta.keys(), types))
    return {"data":data, "consts":consts_arr, "source": filename}



def extend_metadata(log):
    """Produce a new log dictionary with standard metadata
    that may have been missing from the original.  Always
    returns a new dictionary (shallow copy of the original).
    """
    
    dtype = log["data"].dtype
    log = log.copy()
    if "mins" not in log:
        log["mins"]  = np.array(tuple([np.min(log["data"][name]) for name in dtype.names]), dtype=dtype)
        
    if "maxes" not in log:
        log["maxes"] = np.array(tuple([np.max(log["data"][name]) for name in dtype.names]), dtype=dtype)
        
    return log



def print_file(filename, **kwargs):
    "Load a log file and print it; kwargs are the same as print_log"
    try:
        log = np.load(filename)
    except:
        log = load_as_numpy(filename)
        
    print_log(log, **kwargs)
        
    
def print_log(log, ensure_widths=False, lines=-1, delimit=" | "):
    """Print the contents of a log dictionary to std out.
    
    Options:
    delimit -- Field delimiter Default is " | ".
    ensure_widths -- Make sure columns widths are wide enough for all 
                     entries (may require a scan of the data to find the widest entry)
                     Default False.
    lines -- Stop after printing the set number of lines (or end of file).
             Default -1 for print all data.
    """

    data = log["data"]
    names = data.dtype.names
    name_lens = [len(str(name)) for name in names]
    
    if ensure_widths and (not "maxes" in log or not "mins" in log):
        log = extend_metadata(log)
        
    maxes = [len(str(e)) for e in log["maxes"].tolist()] if "maxes" in log else name_lens
    mins = [len(str(e)) for e in log["mins"].tolist()] if "mins" in log else name_lens

    pads = [max(col) for col in zip(name_lens, maxes, mins)]
    
    format = delimit.join(["{%d: >%d}"%(i,pad) for (i, pad) in enumerate(pads)])
    print(format.format(*names))
        
    for (i, line) in enumerate(data):
        if lines > 0 and i > lines: break
        print(format.format(*line))



def print_metadata(log, extend=False):
    """Print the log metadata
    extend -- Run extend_metadata. Default False.
    """
    if extend: log=extend_metadata(log)
    
    names = set(log.keys())
    if "data" in names: 
        data = log["data"]
        names.remove("data")
        print("names: {0}".format(", ".join(data.dtype.names)))
        print("Entry count: {0}".format(len(data)))

    if "consts" in names:
        consts = log["consts"]
        names.remove("consts")
        print("Consts:")
        for name in consts.dtype.names:
            print("   {0} : {1}".format(name, consts[name][0]))
        
    names = sorted(list(names))
    for name in names:
        print("%s : %s" % (name, log[name]))



def merge_precondition(logs, force_drop=[], force_keep={}):
    """
    Drop a field (if present) from each table.
    Add a field (if missing) to every table, using the provided default.
    
    This method is used to improve merge results by providing 
    greater control over what happens to specific fields.
    
    TODO: Add new constants to complement 'force_keep'
    """
    
    def filter_fields(log):
        if len(force_drop) == 0: return log.copy()
        data = log["data"]
        names = [n for n in data.dtype.names if n not in force_drop]
        data = data[names]
        log = log.copy()
        log["data"] = data
        return log
    
    def diff(one, two): return [item for item in one if item not in two]
    
    def extend_fields(log):
        if len(force_keep) == 0: return log
        data = log["data"]
        new_fields = diff(force_keep.keys(), data.dtype.names)
        new_vals = tuple([force_keep[f] for f in new_fields])
        dtype = [(name, np.dtype(type(val))) for (name, val) in zip(new_fields, new_vals)]
        extension = np.empty(len(data), dtype=dtype)
        extension.fill(new_vals)
        data = merge_arrays((data, extension), usemask=False,flatten = True)
        log = log.copy()
        log["data"] = data
        return log

    logs = [extend_fields(filter_fields(log)) for log in logs]
    
    return logs


def merge(*logs, **kwargs):
    """Merge all data from the different files into a single table.
    Field shared across all files are kept.  
    Fields not shared between files are dropped (a message is printed to indicate dropped fields.)

    Constants with the same value across all files are kepts as constants.
    Constants present in only *some* files are dropped.
    Constants present in all files, but with different values are converted into a column
       with the value for each row taken from the constant from the corresponding file.
    If the a column with the constant's name name is already present, 
      a new name is generated that starts with the constant field name. 
    
    kwargs:
    * quiet -- Dont' print info about dropped fields/constants
    * drop -- Force drop specific fields, regardless if they are shared (list fo names)
    * keep -- Keep fields, even if they are not shared (dictionary of field/default value pairs)
    """
    quiet = kwargs.get("quiet", False)
    force_drop = kwargs.get("drop", [])
    force_keep = kwargs.get("keep", {})
    
    if len(force_drop) > 0 or len(force_keep) > 0:
        logs = merge_precondition(logs, force_drop, force_keep)
    
    def gen_name(base, taken):
        if base not in taken: return base
        for i in itertools.count():
            if base+"_c"+str(i) not in taken: return base+"_c"+str(i)
        
    data = [log["data"] for log in logs]
    consts = [log["consts"] for log in logs]
    
    #Reduce logs to just shared fields
    shared_fields = reduce(lambda acc, names: acc.intersection(set(names)),
                           [d.dtype.names for d in data],
                           set(data[0].dtype.names))
    
    if len(shared_fields) == 0: raise ValueError("No fields shared between all logs.  Merge aborted")
    shared_fields = [n for n in data[0].dtype.names if n in shared_fields] # Preservers order

    if not quiet:
        kept = set(shared_fields)
        print("## Merge dropping fields:")
        for (i, log) in enumerate(logs):
            dropped = set(log["data"].dtype.names) - kept
            label =  log.get("source", i) if "source" in log else i
            print("##\ttable {0}: {1}".format(label, ", ".join(list(dropped))))
    
    data = [d[shared_fields] for d in data]
    
    #Collect/merge metadata values
    def merge_consts(acc, consts):
        for name in consts.dtype.names:
            val = consts[name][0]
            current = acc.get(name, dict())
            current[val] = current.get(val, 0) +1
            acc[name] = current
        return acc

    all_consts = reduce(merge_consts,
                        [c for c in consts],
                        dict())
    
    drop = [k for (k,v) in all_consts.items() if len(v)==1 and v[v.keys()[0]] != len(logs)]
    retain = [k for (k,v) in all_consts.items() if len(v)==1 and v[v.keys()[0]] == len(logs)]
    duplicate = [k for (k,v) in all_consts.items() if len(v)>1]
    
    if not quiet:
        print("## Merge dropping constants:", drop)

    def extend(data, const, duplicate):
        if len(duplicate) == 0: return data
        new_vals = tuple([const[f][0] for f in duplicate])
        new_fields = [gen_name(field, data.dtype.names) for field in duplicate]
        dtype = [(name, np.dtype(type(val))) for (name, val) in zip(new_fields, new_vals)]
        extension = np.empty(len(data), dtype=dtype)
        extension.fill(new_vals)
        data = merge_arrays((data, extension), usemask=False, flatten = True)
        return data

    extended = [extend(d, c, duplicate) for (d, c) in zip(data, consts)]
        
    #Merge and return
    return {"data": np.concatenate(extended),
            "consts": consts[0][retain],
            "source": [log.get("source", "---") for log in logs]}

def load_all(*filenames, **kwargs):
    "Load and merge all passed files.  kwargs are passed to merge."
    arrays = [load_as_numpy(name) for name in filenames]
    return merge(*arrays, **kwargs)


import unittest
def test():
    """Run test for HPX.
       NOTE: Test are only successful if you're in a path so 
             ./test_data/ resolves to the proper test data.
    """

    import sys
    suite = unittest.TestLoader().loadTestsFromTestCase(TestLogTools)
    unittest.TextTestRunner(verbosity=2).run(suite)
    sys.argv = ["program", 
                "./test_data/PARCEL_RESEND.log", "./test_data/PARCEL_SEND.log", "./test_data/PARCEL_RUN.log",
               "--compress", "-q"]
    main()
    
    
class TestLogTools(unittest.TestCase):
    def setUp(self):
        self.send = load_as_numpy("./test_data/PARCEL_SEND.log")
        self.run = load_as_numpy("./test_data/PARCEL_RUN.log")
        self.resend = load_as_numpy("./test_data/PARCEL_RESEND.log")
    
    def test_read_length(self):
        self.assertEqual(len(self.send["data"]), 12, "Data array length not as expected")
        self.assertEqual(len(self.run["data"]), 19, "Data array length not as expected")
        self.assertEqual(len(self.resend["data"]), 0, "Zero-length array not properly loaded")

    def test_source_set(self):
        self.assertEqual(self.send["source"], "./test_data/PARCEL_SEND.log")
        self.assertEqual(self.run["source"], "./test_data/PARCEL_RUN.log")
        self.assertEqual(self.resend["source"], "./test_data/PARCEL_RESEND.log")

    def test_constants_found(self):
        self.assertEqual(len(self.send["consts"][0]), 3)
        self.assertEqual(len(self.run["consts"][0]), 3)
        self.assertEqual(len(self.resend["consts"][0]), 3)

    def test_simple_merge(self):
        merged = merge(self.send, self.run, quiet=True)
        self.assertEqual(len(merged["data"]), len(self.send["data"])+len(self.run["data"]))
        self.assertEqual(merged["data"].dtype.names, ('worker', 'nanoseconds', 'id', 'action', 'size', 'id_c0'))
        self.assertEqual(merged["consts"].dtype.names, ('class', 'rank'))

    def test_merge_with_empty(self):
        merged = merge(self.send, self.run, self.resend, quiet=True)
        self.assertEqual(len(merged["data"]), len(self.send["data"])+len(self.run["data"]))
        self.assertEqual(merged["data"].dtype.names, ('worker', 'nanoseconds', 'id', 'action', 'size', 'id_c0'))
        self.assertEqual(merged["consts"].dtype.names, ('class', 'rank'))
        
    def test_merge_drop(self):
        modrun = self.run.copy()
        modrun["consts"] = np.array([(1, 0, 4)], 
                                 dtype=[('class', '<i4'), ('rank', '<i4'), ('DROP', '<i4')])
        remerged = merge(self.send, modrun, quiet=True)
        merged = merge(self.send, self.run, quiet=True)
        self.assertEqual(len(remerged["data"]), len(merged["data"]))
        self.assertEqual(len(remerged["data"].dtype), len(merged["data"].dtype)-1)
        self.assertEqual(len(remerged["consts"]), len(merged["consts"]))
    
    
    def test_condition_drop(self):
        (send, run, resend) = merge_precondition([self.send, self.run, self.resend], 
                                                 force_drop=["worker"])      
        self.assertFalse("worker" in send["data"].dtype.names)
        self.assertFalse("worker" in run["data"].dtype.names)
        self.assertFalse("worker" in resend["data"].dtype.names)
        
    def test_condition_default(self):
        (send, run, resend) = merge_precondition([self.send, self.run, self.resend], 
                                                 force_keep={"some": 3, "other": 4.2})
        self.assertTrue("some" in send["data"].dtype.names)
        self.assertTrue("some" in run["data"].dtype.names)
        self.assertTrue("some" in resend["data"].dtype.names)
        self.assertTrue("other" in send["data"].dtype.names)
        self.assertTrue("other" in run["data"].dtype.names)
        self.assertTrue("other" in resend["data"].dtype.names)

        self.assertTrue(np.all(send["data"]["some"]==3))
        self.assertTrue(np.all(send["data"]["other"]==4.2))
        self.assertTrue(np.all(run["data"]["some"]==3))
        self.assertTrue(np.all(run["data"]["other"]==4.2))
        self.assertTrue(np.all(resend["data"]["some"]==3))
        self.assertTrue(np.all(resend["data"]["other"]==4.2))

def main():
    import argparse
    parser = argparse.ArgumentParser(description="""Process HPX log files (Load, Save as numpy, merge, etc.)
                                                    This package can be used as a command-line tool (e.g., 'python hpxlog ...')
                                                    or as a module for building further tools (after 'pip install hpxlog').""", )
    parser.add_argument("files", nargs='+', 
                        help="HPX log files (or numpy files) to load/merge")
    parser.add_argument("--quiet", "-q", action='store_true', 
                        help="Suppress non-table data in the output")
    parser.add_argument("--out", "-o", nargs='?',
                        help="File to save numpy result")
    parser.add_argument("--compress", "-c", action='store_true',
                        help="Compress output (when saving to file)")

    parser.add_argument("--meta", action='store_true',
                       help="If present, only prints the (merged) metadata")

    parser.add_argument("--delimit", nargs='?', default="|", 
                        help="Column delimiter (for console output)")
    parser.add_argument("--ensure", action='store_true',
                        help="""Ensure all column widths are sufficient for all values observed 
                                (may require two passes over the data)""")

    args = parser.parse_args()
    merged = load_all(*args.files, quiet=args.quiet)

    if args.meta:
        print_metadata(merged)
    elif args.out is None:
        print_log(merged, delimit=args.delimit, ensure_widths=args.ensure)
    else:
        save_numpy(args.out, merged, compress=args.compress)

if __name__ == '__main__':
    main()

