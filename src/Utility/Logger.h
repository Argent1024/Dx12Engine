#pragma once
#include "stdafx.h"
#include "Math/Transform.h"

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

	static void Log(Math::Vector3& T, std::string str="") {
		if (str.size() != 0) {
			std::cout << str << " " << T << std::endl;
		}
		else {
			std::cout << T << std::endl;
		}
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