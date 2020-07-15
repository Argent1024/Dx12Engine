#pragma once
#include "stdafx.h"

class LogObject {

public:

	virtual std::string Log() const 
	{
		return "Log object not implemented";
	};

};


class Logger
{
	// TODO write to file
	// void Initialize(std::string file) {}
public:

	static void Initialize()
	{
		// Print to console
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);  
		Logger::Log("Logger Initialized");
	}

	static void Log(const char* str) {
		std::cout << str << std::endl;
	}

	static void Log(std::string& str) {
		std::cout << str << std::endl;
	}

	static void Log(const LogObject& obj) 
	{
		std::cout << obj.Log() << std::endl;
	}

};