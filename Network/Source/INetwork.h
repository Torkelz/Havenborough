#pragma once

class INetwork
{
public:
	/**
	* Create a pointer from which the network library can be accessed.
	*/
	__declspec(dllexport) static INetwork *create();


};