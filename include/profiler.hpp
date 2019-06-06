#pragma once

#ifndef PROFILER_HEADER_H_
#define PROFILER_HEADER_H_

#include <string>
#include "stringutil.hpp"
#include <vector>
#include <utility>

#include <chrono>

#if defined( BOOST_DISABLE_CURRENT_FUNCTION )
# define CURRENT_FUNCTION "(unknown)"
#elif defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
# define CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
# define CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
# define CURRENT_FUNCTION __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
# define CURRENT_FUNCTION __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
# define CURRENT_FUNCTION __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
# define CURRENT_FUNCTION __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
# define CURRENT_FUNCTION __func__
#else
# define CURRENT_FUNCTION "(unknown)"
#endif

#define PASTER(x,y) x ## _ ## y
#define EVALUATOR(x,y)  PASTER(x,y)
#define NAME(fun) EVALUATOR(fun, __COUNTER__)

#define ProfileDeclare Profiler NAME(_) (__func__, CURRENT_FUNCTION, __FILE__, std::to_string(__LINE__ - 1));

namespace chrono = std::chrono;
using timepoint_t = chrono::high_resolution_clock::duration;

struct StackFrame {
	std::string funcName;
	std::string richFuncName;
	std::string file;
	std::string line;
	timepoint_t callTime;

	StackFrame(std::string _fn, std::string _rfn, std::string _f,
			   std::string _l, timepoint_t _t) : funcName(std::move(_fn)), richFuncName(std::move(_rfn)),
												file(std::move(_f)), line(std::move(_l)),
												callTime(std::move(_t)) {
	}
};

class Profiler {
public:
	using callstack_t = std::vector<StackFrame>;
private:
	static std::string targetFile;
	static callstack_t callstack;
	static int _verbosity;
	static int _traceback;
	static std::string buffer;

	friend void dumpBuffer(bool exiting);
	friend void partialDump();

	static timepoint_t totalSpent;
public:
	Profiler(std::string func, std::string prettyFunc, std::string file, std::string line);
	~Profiler();

	static std::string& target() {
		return targetFile;
	}

	static void target(std::string targetFil);

	static int verbosity() {
		return _verbosity;
	}

	static void verbosity(int newV) {
		_verbosity = newV;
	}

	static timepoint_t spent() {
		return totalSpent;
	}

	static int traceback() {
		return _traceback;
	}

	static void traceback(int newV) {
		_traceback = newV;
	}

	static const callstack_t& stack() {
		return callstack;
	}
};

void dumpBuffer(bool exiting);

#endif // PROFILER_HEADER_H_
