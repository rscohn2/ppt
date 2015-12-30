/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2015 Intel Corporation. All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */

/* ===================================================================== */
/*
  @ORIGINAL_AUTHOR: Robert Cohn
*/

/* ===================================================================== */
/*! @file
 *  This file contains a tool that generates instructions traces with values.
 *  It is designed to help debugging.
 */



#include "pin.H"
#include "portability.H"
#include "string.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <set>
#include "../helper/ppt_helper_interface.h"

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,         "pintool",
    "o", "ppt.out", "trace file");
KNOB<BOOL>   KnobPid(KNOB_MODE_WRITEONCE,                "pintool",
    "i", "0", "append pid to output");
KNOB<BOOL>   KnobFlush(KNOB_MODE_WRITEONCE,                "pintool",
    "flush", "0", "Flush output after every instruction");


/* ===================================================================== */

INT32 Usage()
{
    cerr <<
        "This pin tool collects an instruction trace for debugging\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

LOCALVAR std::ofstream out;
LOCALVAR INT32 indent = 0;

LOCALFUN VOID Flush()
{
    if (KnobFlush)
        out << flush;
}

LOCALFUN VOID Indent()
{
    for (INT32 i = 0; i < indent; i++)
    {
        out << "| ";
    }
}

LOCALVAR VOID* getAttrString_p = 0;
LOCALVAR VOID* asString_p = 0;
LOCALVAR map<void*, char*> entry_map;
LOCALVAR set<char const*> entries;

LOCALFUN void* getAttrString(CONTEXT* ctxt, char const* field, void* obj)
{
  ASSERTX(getAttrString_p);
  void* ret;
  PIN_CallApplicationFunction(ctxt, PIN_ThreadId(), CALLINGSTD_DEFAULT, AFUNPTR(getAttrString_p), NULL,
			      PIN_PARG(void*), &ret, PIN_PARG(void*), obj, PIN_PARG(char*), field, PIN_PARG_END());
  return ret;
}
  
LOCALFUN char* asString(CONTEXT* ctxt, void* obj)
{
  ASSERTX(asString_p);
  void* ret;
  PIN_CallApplicationFunction(ctxt, PIN_ThreadId(), CALLINGSTD_DEFAULT, AFUNPTR(asString_p), NULL,
			      PIN_PARG(void*), &ret, PIN_PARG(void*), obj, PIN_PARG_END());
  return static_cast<char*>(ret);
}
  
LOCALFUN VOID EnterEvalFrame(CONTEXT* ctxt, VOID* frame) 
{
  indent++;
  void* co = getAttrString(ctxt, "f_code", frame);
  void* name = getAttrString(ctxt, "co_name", co);
  char* str = asString(ctxt, name);
  Indent();
  out << str << endl;
  Flush();
}

LOCALFUN VOID LeaveEvalFrame(VOID* v) 
{
  indent--;
}

/* ===================================================================== */

LOCALFUN VOID recordEntry(char const* name, BOOL clear)
{
  // if we are entering from python clear all the names
  if (clear)
    entries.clear();

  // only print a name once for each entrace from python
  if (entries.find(name) != entries.end())
    return;
  entries.insert(name);

  Indent();
  out << "  " << name << endl;
  Flush();
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
      RTN_InsertCall(rtn, IPOINT_BEFORE, AFUNPTR(recordEntry), IARG_PTR, n, IARG_BOOL, false, IARG_END);
      RTN_Close(rtn);
    }
  }
}

LOCALFUN void trace_method(struct trace_method_info* i)
{
  string s = string(i->module_name) + ":" + i->method_name;
  entry_map[i->address] = strdup(s.c_str());
}

LOCALFUN VOID receiveHelp(INT n, VOID* args) 
{
  switch(n) {
  case TRACE_METHOD:
    trace_method(static_cast<struct trace_method_info*>(args));
    break;
  default:
    cerr << "Unknown help: " << n << endl;
  }
}

// This routine is executed for each image.
LOCALFUN VOID ImageLoad(IMG img, VOID *)
{
    RTN rtn = RTN_FindByName(img, "PyObject_GetAttrString");
    if ( RTN_Valid( rtn ))
      getAttrString_p = reinterpret_cast<VOID *>(RTN_Address(rtn));

    rtn = RTN_FindByName(img, "PyString_AsString");
    if ( RTN_Valid( rtn ))
      asString_p = reinterpret_cast<VOID *>(RTN_Address(rtn));

    rtn = RTN_FindByName(img, "ppt_to_tool");
    if ( RTN_Valid( rtn )) {
      RTN_Open(rtn);
      RTN_InsertCall(rtn, IPOINT_BEFORE, AFUNPTR(receiveHelp), IARG_G_ARG0_CALLEE, IARG_G_ARG1_CALLEE, IARG_END);
      RTN_Close(rtn);
    }

    if (IMG_Name(img).find("/ext/lib/libmkl") != string::npos)
      instrumentEntryPoints(img);
}

LOCALFUN VOID Trace(TRACE trace, VOID* v) 
{
  RTN rtn = TRACE_Rtn(trace);
  BOOL inEvalFrame = RTN_Valid(rtn) && RTN_Name(rtn) == "PyEval_EvalFrameEx";

  for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
  {
    for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
    {
      void* address = reinterpret_cast<void*>(INS_Address(ins));
      if (entry_map.count(address)) {
	INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(recordEntry), IARG_PTR, entry_map[address], IARG_BOOL, true, IARG_END);
      }

      if (inEvalFrame) {
	if (INS_Address(ins) == RTN_Address(rtn)) {
	  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(EnterEvalFrame), IARG_CONTEXT, IARG_G_ARG0_CALLEE, IARG_END);
	}

	if (INS_IsRet(ins)) {
	  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(LeaveEvalFrame), IARG_END);
	}
      }
    }
  }

}

LOCALFUN VOID Fini(int, VOID * v)
{
    out << "# $eof" <<  endl;

    out.close();
}

/* ===================================================================== */

int main(int argc, CHAR *argv[])
{
    PIN_InitSymbols();

    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    
    string filename =  KnobOutputFile.Value();

    if( KnobPid )
    {
        filename += "." + decstr( getpid_portable() );
    }

    // Do this before we activate controllers
    out.open(filename.c_str());
    out << hex << right;
    out.setf(ios::showbase);

    TRACE_AddInstrumentFunction(Trace, 0);
    IMG_AddInstrumentFunction(ImageLoad, 0);

    PIN_AddFiniFunction(Fini, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
