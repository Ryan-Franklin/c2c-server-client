
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <ctime>
#include <sys/time.h>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <stdlib.h>

#define MAXBUF 1024
#define MAXAGENTS 6

using namespace std;

bool isAgent(string*, int, string);
void log(string);
void printServerInfo(int);

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        cerr << "Error: please enter a port number" << endl;
        return -1;
    }
    struct sockaddr_in servAddr,
                       clientAddr;

    char buffer[MAXBUF];

    string ipAddr, 
           actionReply,
           agentList[MAXAGENTS];
   
    int addrlen = sizeof(clientAddr), 
                port = atoi(argv[1]), 
                agentCount = 0, 
                sd, 
                newSd;

    time_t timeSheets[MAXAGENTS];

    if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("simplex-talk: socket");
        exit(1);
    }
    cout << "Socket Created" << endl;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    if((bind(sd, (struct sockaddr*)&servAddr, sizeof(servAddr))) < 0)
    {
        perror("simplex-talk: bind");
        exit(1);
    }

    if((listen(sd, 10)) < 0)
    {
        perror("simplex-talk: listen");
        exit(1);
    }
    printServerInfo(port);
    memset(buffer, 0, MAXBUF);

    while(1)
    {
        if((newSd = accept(sd, (struct sockaddr*)&clientAddr, (socklen_t*)&addrlen)) < 0)
        {
            perror("simplex-talk: accept");
            exit(1);
        }

        ipAddr = inet_ntoa(clientAddr.sin_addr);

        recv(newSd, buffer, MAXBUF, 0);

        if(strcmp(buffer, "#JOIN") == 0)
        {
            log("Received a \"#JOIN\" action from agent \"" + ipAddr + "\"");

            if(isAgent(agentList, agentCount, ipAddr))
            {
                actionReply = "$ALREADY MEMBER";
                write(newSd, actionReply.c_str(), actionReply.length());
                log("Responded to agent \"" + ipAddr + "\" with \"" + actionReply + "\"");
            }
            else
            {   
                actionReply = "$OK";
                agentList[agentCount] = ipAddr;
                timeSheets[agentCount] = time(NULL);
                agentCount++; 
                write(newSd, actionReply.c_str(), actionReply.length());
                log("Responded to agent \"" + ipAddr + "\" with \"" + actionReply + "\"");
            }
        }
        else if(strcmp(buffer, "#LEAVE") == 0)
        {
            log("Received a \"#LEAVE\" action from agent \"" + ipAddr + "\"");

            if(isAgent(agentList, agentCount, ipAddr))
            {
                int index = 0;
                bool found = false;
                while(!found && (index < agentCount))
                {
                    if(agentList[index] == ipAddr)
                        found = true;
                    else
                        index++;
                }
                for(int i = index; i < agentCount - 1; i++)
                {
                    agentList[i] = agentList[i+1];
                    timeSheets[i] = timeSheets[i+1];
                }
                actionReply = "$OK";
                
                write(newSd, actionReply.c_str(), actionReply.length());
                log("Responded to agent \"" + ipAddr + "\" with \"" + actionReply + "\"");
                
                agentCount--;
            }
            else
            {
                actionReply = "$NOT MEMBER";
                write(newSd, actionReply.c_str(), actionReply.length());
                log("Responded to agent \"" + ipAddr + "\" with \"" + actionReply + "\"");
            }
        }
        else if(strcmp(buffer, "#LIST") == 0) {
            log("Received a \"#LIST\" action from agent \"" + ipAddr + "\"");

            if(isAgent(agentList, agentCount, ipAddr))
            {
                double dbl;
                ostringstream strs;
                          
                for(int i = 0; i < agentCount; i++)
                {
                    dbl = difftime(time(NULL), timeSheets[i]);
                    strs << dbl;
                    string str = strs.str();  
                    actionReply = "<" + agentList[i] + ", " + str + ">";

                    write(newSd, actionReply.c_str(), actionReply.length());
                }
                log("Responded to agent \"" + ipAddr + "\" with list of active agents");
            }
            else
            {
                log("No actionReply is supplied to agent \"" + ipAddr + "\" (agent not active)");
            }
        }
        else if(strcmp(buffer, "#LOG") == 0)
        {

            log("Received a \"#LOG\" action from agent \"" + ipAddr + "\"");

            if(isAgent(agentList, agentCount, ipAddr))
            {
                ifstream inFile("log.txt");
                if(!inFile)
                {
                    cerr << "ERROR: Unable to open \"log.txt\"" << endl;
                    return -1;
                }
                while(!(inFile.eof()))
                {
                    memset(buffer, 0, MAXBUF);
                    inFile.read(buffer, MAXBUF);
                    write(newSd, buffer, MAXBUF);
                }
                inFile.close();
                log("Responded to agent \"" + ipAddr + "\" with \"log.txt\"");
            }
            else
            {
                log("No reply is supplied to agent \"" + ipAddr + "\" (agent not active)");
            }
        }
        close(newSd);  
    }
    close(sd);
    return 0;
}

bool isAgent(string A[], int size, string target)
{
	int j;
	{
	for(j=0; j < size; j++)
		if(A[j] == target)
			return true;
		else 
            return false;
	}
}

void log(string reply) 
{
    ofstream logFile("log.txt", ios::app);
    char buffer[80],
         timeStamp[84] = "";

    timeval curTime;
    gettimeofday(&curTime, NULL);

    strftime(buffer, 80, "TIME: %Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
  
    sprintf(timeStamp, "%s:%d", buffer, curTime.tv_usec / 1000);

    logFile << "\"" << timeStamp << "\" " << reply << "\n";

    logFile.close();
}

void printServerInfo(int port)
{
    printf("\n\n");
    printf("eros.cs.txstate.edu IP Address: 147.26.231.153 \n");
    printf("zeus.cs.txstate.edu IP Address: 147.26.231.156 \n\n");
    printf("Server Port: %i \n\n\n", port);
}