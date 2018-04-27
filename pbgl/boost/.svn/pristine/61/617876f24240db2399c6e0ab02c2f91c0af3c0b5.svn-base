#include <am++/am++.hpp>
#include <am++/mpi_transport.hpp>
#include <boost/timer/timer.hpp>
#include <boost/program_options.hpp>
#include <scorep/SCOREP_User.h>

//#include <boost/config.hpp>
//#include <am++/basic_coalesced_message_type.hpp>
//#include <am++/counter_coalesced_message_type.hpp>
//#include <boost/pool/object_pool.hpp>
//#include <boost/lexical_cast.hpp>
//#include <boost/assert.hpp>
//#include <stdio.h>
//#include <string>
//#include <sstream>
//#include <numeric>
//#include <unistd.h>

using namespace boost;

typedef double time_type;
std::string print_time(time_type t)
{             
	std::ostringstream out;
	out << std::setiosflags(std::ios::fixed) << std::setprecision(2) << t;
	return out.str();
}             

#define POLY 0x0000000000000007ULL
#define PERIOD 1317624576693539401LL

//????
struct owner_map_type {
	amplusplus::transport::rank_type nprocs; 
	uint64_t N;
	owner_map_type(const uint64_t &N, const amplusplus::transport::rank_type &nprocs) : 
		N(N), nprocs(nprocs) {}
};
template<typename T> inline amplusplus::transport::rank_type get(const owner_map_type & map, const T & key) { 
	//std::cout << ": key = " << std::hex << key << std::dec << 
	//	"shift bits = " << (int)log2((double)map.N) << " dest = " << 
	//	(key >> (int)log2((double)map.N) & map.nprocs - 1) << std::endl;
	return key >> (int)log2((double)map.N) & map.nprocs - 1; 
}

template<class MessageGenerator = 
                amplusplus::simple_generator<amplusplus::basic_coalesced_message_type_gen> >
	class hpcc_random_access {
		public:
			hpcc_random_access(uint64_t N, int iters, amplusplus::transport & transport, 
					MessageGenerator message_gen = 
						MessageGenerator(amplusplus::basic_coalesced_message_type_gen(1 << 12))) :
	 			N(N), iters(iters), transport(transport), owner(N, transport.size()),
				msg(message_gen, transport, owner, amplusplus::no_reduction) {
					//init table
					hpcc_table = (uint64_t *)malloc(N * sizeof(uint64_t));
					uint64_t starts = transport.rank() * N;
					for(uint64_t i = 0; i < N; i++)
						hpcc_table[i] = i + starts;

					msg.set_handler(msg_handler(*this));

					local_update_counter = 0;
				}
			void run() {
				SCOREP_USER_REGION( "run()", SCOREP_USER_REGION_TYPE_FUNCTION )
				AMPLUSPLUS_WITH_THREAD_ID(0) {
					amplusplus::scoped_epoch epoch(transport);

					uint64_t key = HPCC_starts(iters * transport.rank() * N);
					//uint64_t key = transport.rank() * N;
					std::cout << transport.rank() << ": key = " << std::hex << key << " " <<
					 	(key >> (int)log2((double)N) /*& transport.size() - 1*/) << std::dec << "\n";

					//if (transport.rank() == 1)
					for(int k = 0; k < iters; k++)
						for(uint64_t i = 0; i < N; i++) {
							key = key << 1 ^ ((int64_t) key < 0 ? POLY : 0);
							//msg.send(transport.rank() << (int)log2((double)N)/* + (key % N)*/);
							msg.send(key);
						}

					std::cout << transport.rank() << ": message generation complete\n";
				}
			}
			uint64_t get_local_update_counter() { return local_update_counter; }
		private:
			amplusplus::transport& transport;
			uint64_t *hpcc_table;
			uint64_t N;
			int iters;
      struct msg_handler;
			uint64_t local_update_counter;

		 	typedef struct owner_map_type OwnerMap;
      OwnerMap owner;

      typedef typename MessageGenerator::template call_result<
				uint64_t, msg_handler, OwnerMap, amplusplus::no_reduction_t>::type message_type;
			message_type msg;

			/** random number generator */
			uint64_t HPCC_starts(int64_t n) {
				int i, j;
				uint64_t m2[64];
				uint64_t temp, ran;
				while (n < 0) n += PERIOD;
				while (n > PERIOD) n -= PERIOD;
				if (n == 0) return 0x1;
				temp = 0x1;
				for (i = 0; i < 64; i++) {
					m2[i] = temp;
					temp = temp << 1 ^ ((int64_t) temp < 0 ? POLY : 0);
					temp = temp << 1 ^ ((int64_t) temp < 0 ? POLY : 0);
				}
				for (i = 62; i >= 0; i--)
					if (n >> i & 1)
						break;

				ran = 0x2;
				while (i > 0) {
					temp = 0;
					for (j = 0; j < 64; j++)
						if (ran >> j & 1)
							temp ^= m2[j];
					ran = temp;
					i -= 1;
					if (n >> i & 1)
						ran = ran << 1 ^ ((int64_t) ran < 0 ? POLY : 0);
				}
				return ran;
			}
	};

template <typename MessageGenerator>
struct hpcc_random_access <MessageGenerator>::msg_handler {

	msg_handler() : self(NULL) {}
	msg_handler(hpcc_random_access& self) : self(&self) {}

	void operator() (const uint64_t & key) const {
		SCOREP_USER_REGION( "operator()", SCOREP_USER_REGION_TYPE_FUNCTION )
		uint64_t offset = key & (self->N - 1);
		self->hpcc_table[offset] ^= key;
		//std::cout << self->transport.rank() << ": update key = " << std::hex << key << std::endl;
		self->local_update_counter++;
	}

	protected:
	hpcc_random_access* self;
};


double run_random_access(amplusplus::transport &trans, amplusplus::transport &barrier_trans, 
		int scale, int iters) {
	// allocate hpcc table
	hpcc_random_access<> ra(1 << scale, iters, trans);

	if (trans.rank() == 0) {
		std::cout << "HPCC RandomAccess" << std::endl;
	}

	{ amplusplus::scoped_epoch epoch(barrier_trans); }

  timer::cpu_times tstart, tend;
  timer::cpu_timer timer;

  tstart.clear();
  tstart = timer.elapsed();

	ra.run();

  tend = timer.elapsed();

	if (trans.rank() == 0) {
		std::cout << "\tTime elapsed " << (double)(tend.wall - tstart.wall)/1000000000 << " sec" << std::endl;
	}
	std::cout << "\t" << trans.rank() << ": updates " << ra.get_local_update_counter() << std::endl;

	{ amplusplus::scoped_epoch epoch(barrier_trans); }

	return (double)(tend.wall - tstart.wall)/1000000000;
}


int main(int argc, char** argv) {
  amplusplus::environment env = amplusplus::mpi_environment(argc, argv);
  amplusplus::transport trans = env.create_transport();
	trans.set_nthreads(1);
	amplusplus::transport barrier_trans = trans.clone();

	double total_time = 0;
	uint64_t scale = 10;
	uint64_t iters = 4;

	program_options::options_description opts("Supported options");
	opts.add_options()
		("help,h","print usage message")
		("scale,s",program_options::value(&scale), "table size = 2 ^ scale * sizeof(uint64_t)")
		("iter,i",program_options::value(&iters), "iterations")
		;

	program_options::variables_map vm;
	program_options::store(program_options::parse_command_line(argc, argv, opts), vm);
	program_options::notify(vm);

	if (vm.count("help")) {
		//if (process_id(pg) == 0)
		if (trans.rank() == 0)
			std::cout << opts << std::endl;
		return 0;
	}

	if (trans.rank() == 0) {
		std::cout << "\tGlobal table size   = 2^" << scale << " * " << trans.size() << " = " << 
			(1 << scale) * trans.size() << " words\n";
		std::cout << "\tNumber of processors = " << trans.size() << std::endl;
		std::cout << "\tNumber of updates = " << iters * (1 << scale) * trans.size() << std::endl;
	}

	total_time = run_random_access(trans, barrier_trans, scale, iters);

  if (trans.rank() == 0) {
		double gups = 1e-9 * (1 << scale) * trans.size() * iters * 1.0/total_time;
		std::cout << "Done." << std::endl;
		std::cout << "Total RandomAccess time "<< total_time << " " <<  print_time(total_time) << " (" 
			<< print_time(total_time / iters) << " per iteration)  " << "\n";
		std::cout << "[Final] CPU time used = " << print_time(total_time)  << " seconds, " << gups << " GUPS\n";
	}

  return 0;
}
