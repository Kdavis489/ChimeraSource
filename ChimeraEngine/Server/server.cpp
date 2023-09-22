#include "serverTCP.h"

int main()
{
	servertcp server;
	if (server.Start(80))
	{
		server.Accept();
	}
	return 0;
}