/*! @file
 *  A pin tool that collects the set of calls
 */

#include "pin.H"
#include "portability.H"
#include "string.h"
#include <iostream>
#include <set>
#include "../helper/ppt_helper_interface.h"


/* ===================================================================== */

INT32 Usage()
{
    cerr <<
        "This pin tool collects a trace of calls\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}

LOCALVAR set<char const*> cfunctions;
LOCALVAR BOOL tracing_on;
LOCALVAR char const** funca = 0;

/* ===================================================================== */

LOCALFUN VOID recordEntry(char const* name)
{
  if (!tracing_on)
    return;

  cfunctions.insert(name);
}

LOCALFUN VOID startTrace() {
  tracing_on = true;
  cfunctions.clear();
}

LOCALFUN VOID stopTrace(VOID* p) {
  if (funca)
    delete [] funca;
  funca = new char const*[cfunctions.size() + 1];
  unsigned int i = 0;

  for (set<char const* >::iterator it = cfunctions.begin(); it != cfunctions.end(); ++it)
    funca[i++] = *it;
  ASSERT(i == cfunctions.size(), "Error passing back cfunctions");

  *static_cast<char const***>(p) = funca;
  funca[i] = 0;
}

LOCALFUN VOID instrumentEntryPoints(IMG img)
{
  for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
  {
    for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
    {
      RTN_Open(rtn);
      string name = IMG_Name(img) + ":" + RTN_Name(rtn);
      char const* n = strdup(name.c_str());
      RTN_InsertCall(rtn, IPOINT_BEFORE, AFUNPTR(recordEntry), IARG_PTR, n, IARG_END);
      RTN_Close(rtn);
    }
  }
}


LOCALFUN VOID receiveHelp(INT n, VOID* args) 
{
  switch(n) {
  case START_TRACE:
    startTrace();
    break;
  case STOP_TRACE:
    stopTrace(args);
    break;
  default:
    cerr << "Unknown help: " << n << endl;
  }
}

// This routine is executed for each image.
LOCALFUN VOID ImageLoad(IMG img, VOID *)
{
    RTN rtn = RTN_FindByName(img, "ppt_to_tool");
    if ( RTN_Valid( rtn )) {
      RTN_Open(rtn);
      RTN_InsertCall(rtn, IPOINT_BEFORE, AFUNPTR(receiveHelp), IARG_G_ARG0_CALLEE, IARG_G_ARG1_CALLEE, IARG_END);
      RTN_Close(rtn);
    }

    if (IMG_Name(img).find("/ext/lib/libmkl") != string::npos)
      instrumentEntryPoints(img);
}

/* ===================================================================== */

int main(int argc, CHAR *argv[])
{
    PIN_InitSymbols();

    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    
    IMG_AddInstrumentFunction(ImageLoad, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
