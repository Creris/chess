#include "../include/profiler.hpp"
#include <cstdlib>
#include <fstream>
#include <string>

std::string Profiler::targetFile;
Profiler::callstack_t Profiler::callstack = { { "system", "system", "", "0",
												chrono::high_resolution_clock::now().time_since_epoch() } };
int Profiler::_verbosity = 0;
std::string Profiler::buffer;
timepoint_t Profiler::totalSpent;
int Profiler::_traceback = 0;

#include <iostream>

inline void _doCallstack(std::string& output,
						 const Profiler::callstack_t& v) {
	using namespace std::string_literals;

	output += "Traceback:\n";
	for (int i = v.size() - 2; i > 0; --i) {
		std::string name = v[i].funcName;
		if (Profiler::verbosity())
			name = v[i].richFuncName;

		output += "  File \""s + v[i].file.substr(v[i].file.find_last_of('\\') + 1) + "\", line " + v[i].line + ", in " + name + "\n";
	}
}

std::string formatFrame(const StackFrame& frame, const timepoint_t& now) {
	using namespace std::string_literals;

	std::string buffer;
	buffer.reserve(10000);

	if (Profiler::target().size()) {
		auto destructorTime = now - frame.callTime;
		auto _traceback = Profiler::traceback();
		auto _verbosity = Profiler::verbosity();


		std::string name = frame.funcName;
		if (_verbosity)
			name = frame.richFuncName;

		buffer += "File \""s + frame.file.substr(frame.file.find_last_of('\\') + 1)
			+ "\", line " + frame.line + ", in "
			+ name;

		buffer += " (Executed in ";

		static const char* suffixes[] = {
			"ns", "us", "ms", "s"
		};

		long long counter = chrono::duration_cast<chrono::nanoseconds>(destructorTime).count();
		long long bottom = 0;
		int idx = 0;

		while (counter > 1000 && idx < 3) {
			idx++;
			bottom = counter % 1000;
			counter /= 1000;
		}

		buffer += std::to_string(counter + (bottom / 1000.f)) + suffixes[idx] + ").\n";
		
		if (_traceback && Profiler::stack().size() > 2) {
			_doCallstack(buffer, Profiler::stack());
		}

		if (_traceback)
			buffer += "\n";
	}

	return buffer;
}

void partialDump() {
	if (!Profiler::target().size())	return;

	std::ofstream f{ Profiler::target() };
	Profiler::buffer.shrink_to_fit();
	f.write(&Profiler::buffer[0], Profiler::buffer.size());
}

void dumpBuffer(bool exiting) {
	if (!Profiler::target().size())	return;

	auto now = chrono::high_resolution_clock::now().time_since_epoch();

	//Collect unclosed stack values
	if (exiting)
		Profiler::buffer += "\n\nProgram exiting. Remaining stack:\n\n";
		while (Profiler::callstack.size() > 1) {
			Profiler::buffer += formatFrame(Profiler::callstack.back(), now);
			Profiler::callstack.pop_back();
		}

	std::ofstream f{ Profiler::target() };
	Profiler::buffer.shrink_to_fit();
	f.write(&Profiler::buffer[0], Profiler::buffer.size());
	std::string totalSpentTimer = "\n\n";
	totalSpentTimer += "Total time spent composing this file: ";

	static const char* suffixes[] = {
		"ns", "us", "ms", "s"
	};

	long long counter = chrono::duration_cast<chrono::nanoseconds>(Profiler::spent()).count();
	long long bottom = 0;
	int idx = 0;

	while (counter > 1000 && idx < 3) {
		idx++;
		bottom = counter % 1000;
		counter /= 1000;
	}

	totalSpentTimer += std::to_string(counter + (bottom / 1000.f)) + suffixes[idx] + ".\n";

	totalSpentTimer.shrink_to_fit();
	f.write(&totalSpentTimer[0], totalSpentTimer.size());
}

void dumpBuffer() {
	dumpBuffer(true);
}

inline auto atExitRegister = std::atexit(dumpBuffer);

Profiler::Profiler(std::string func, std::string prettyFunc, std::string file, std::string line)
{
	Profiler::callstack.emplace_back(func, prettyFunc, file, line, chrono::high_resolution_clock::now().time_since_epoch());
}

Profiler::~Profiler()
{
	using namespace std::string_literals;

	if (callstack.size() <= 1)	return;
	
	auto now = chrono::high_resolution_clock::now().time_since_epoch();
	if (targetFile.size()) {
		buffer += formatFrame(callstack.back(), now);
		if (buffer.size() > 1024 * 1024) {
			partialDump();
			buffer.clear();
		}
	}

	callstack.pop_back();
	totalSpent += chrono::high_resolution_clock::now().time_since_epoch() - now;
}

void Profiler::target(std::string targetFil)
{
	dumpBuffer(false);
	targetFile = targetFil;
	buffer = "";
}

