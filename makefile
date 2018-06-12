curl: curl.cc 
	clang++ -std=c++11 curl.cc -o curl -lcurl -lboost_regex 
