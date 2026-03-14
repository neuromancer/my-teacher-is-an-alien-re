#include "SC_Question.h"
#include "Message.h"

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

/* Function start: 0x41A150 */ /* DEMO ONLY - no full game match */
void SC_Message_Send(int targetAddress, int sourceAddress, int command, int data,
    int priority, int param1, int param2, int userPtr,
    int param9, int param10)
{
    SendGameMessage(targetAddress, sourceAddress, command, data, priority, param1, param2, userPtr, param9, param10);
}

// Message::Message removed - the actual constructor is SC_Message::SC_Message at 0x4198C0
