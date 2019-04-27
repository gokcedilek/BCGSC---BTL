class threadPool
{
	public:
		unsigned num_t;
		std::vector<std::thread> tPool;
		std::queue jobs;
		threadPool(unsigned num_t): {this.num_t = num_t; tPool.reserve(this.num_t); }
		void start(){
			for(unsigned i=0; i<num_t; ++i){
				tPool[i] = std::thread()
			}
			start-pooling(tPool);
		}
		void end(){
			for(auto thread : tPool){
				if(thread.joinable()){
					thread.join();
				}
			}
		}

	private:
		bool done = false;
		std::mutex privateMutex;
		void process-job(){
			std::lock_guard<std::mutex> lock(privateMutex);
			filter_write(
		}
		void start-pooling(std::vector<std::thread> pool){
			for(auto thread : tPool){
				if(thread.done == true){
					process-job
				}
			}
		}

}

class Worker{
	public:
		template<typename... Args>
		void put-task(Args&&... args){
			tasks.push_back(std::bind(std::forward<Args>(args)...));
		}

	private:
		std::queue tasks;
		std::condition_variable condition;

}

int main(){
	unsigned t = 4;
	Worker workers(t);

	for(auto& 
	
}
