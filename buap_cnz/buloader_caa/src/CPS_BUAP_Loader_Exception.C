#include <string.h>
#include "CPS_BUAP_Loader_Exception.H"
#include <stdio.h>

struct ErrorPair
{
  int    errorCode;
  char * errorText;
};

// The error messages are described with an AFPSHELL user in mind.
// For example, "created" is translated to "defined".

static const ErrorPair errorMessages [] = 
{
  {CPS_BUAP_Loader_Exception::OK, 
    "OK."},
  {CPS_BUAP_Loader_Exception::GENERAL_FAULT, 
    "Error when executing, general fault."},
  {CPS_BUAP_Loader_Exception::INCORRECT_USAGE, 
    "Incorrect usage."},

  {CPS_BUAP_Loader_Exception::JTP_JIDREPREQ_FAILED, 
    "Process could not register its server to JTP DSD."},
  {CPS_BUAP_Loader_Exception::JTP_JEXINITIND_FAILED, 
    "Unable to initiate JTP connection"},
  {CPS_BUAP_Loader_Exception::JTP_JEXINITRSP_FAILED, 
    "Unable to send JTP connection reply"},
  {CPS_BUAP_Loader_Exception::JTP_JEXDATAIND_FAILED, 
    "Unable to send JTP message"},
  {CPS_BUAP_Loader_Exception::JTP_JEXDATAREQ_FAILED, 
    "Unable to send JTP message response"},

  {CPS_BUAP_Loader_Exception::JTP_DNS_INIT_FAILED, 
    "The BULOADER process could not register its server to JTP DNS."},
  {CPS_BUAP_Loader_Exception::INCOMING_CONNECT_FAILED, 
    "The BULOADER process could not accept a new connect."},
  {CPS_BUAP_Loader_Exception::INCOMING_CONFIRM_FAILED, 
    "The BULOADER process could not confirm a new connect."},
  {CPS_BUAP_Loader_Exception::UNEXPECTED_JTP_REPLY, 
    "Unexpected reply for the previous JTP message."},
  {CPS_BUAP_Loader_Exception::RELADMPAR_LOCK_FAILED, 
    "Could not lock reladmparfile."},
  {CPS_BUAP_Loader_Exception::SERVER_DEAD, 
    "No contact with server part of software."},
  {CPS_BUAP_Loader_Exception::FATALERROR, 
    "Fatal error, contact your system programmer."}
};

//------------------------------------------------------------------------------
// CPS_BUAP_Loader_Exception
//------------------------------------------------------------------------------

CPS_BUAP_Loader_Exception::CPS_BUAP_Loader_Exception ()
{
  error_ = CPS_BUAP_Loader_Exception::FATALERROR;
  oserror_ = 0;
  file_ = "";
  line_ = 0;
  comment_ = "";
}
 

//------------------------------------------------------------------------------
// CPS_BUAP_Loader_Exception
//------------------------------------------------------------------------------

 CPS_BUAP_Loader_Exception::CPS_BUAP_Loader_Exception (errorCode error)
{
  error_ = error;
  oserror_ = 0;
  file_ = "";
  line_ = 0;
  comment_ = "";
}


//------------------------------------------------------------------------------
// CPS_BUAP_Loader_Exception
//------------------------------------------------------------------------------

 CPS_BUAP_Loader_Exception::CPS_BUAP_Loader_Exception (errorCode error,
				       const char* comment)
{
  error_ = error;
  oserror_ = 0;
  file_ = "";
  line_ = 0;
  comment_ = comment;
}


//------------------------------------------------------------------------------
// CPS_BUAP_Loader_Exception
//------------------------------------------------------------------------------

 CPS_BUAP_Loader_Exception::CPS_BUAP_Loader_Exception (errorCode error,
                                       int oserror,
				       const char*  file, 
				       int line,
				       const char* comment)
{
  error_ = error;
  oserror_ = oserror;
  file_ = file;
  line_ = line;
  comment_ = comment;
}


//------------------------------------------------------------------------------
// CPS_BUAP_Loader_Exception
//------------------------------------------------------------------------------

 CPS_BUAP_Loader_Exception::CPS_BUAP_Loader_Exception (errorCode error,
                                       int oserror,
				       const char*  file, 
				       int line,
				       istream & istr)
{
  error_ = error;
  oserror_ = oserror;
  file_ = file;
  line_ = line;

  RWCString str;
  str.readString (istr); // Shall not leak memory
  comment_ += str;
}


//------------------------------------------------------------------------------
// CPS_BUAP_Loader_Exception
//------------------------------------------------------------------------------

 CPS_BUAP_Loader_Exception::CPS_BUAP_Loader_Exception (const CPS_BUAP_Loader_Exception& ex) 
{
  error_ = ex.error_;
  oserror_ = ex.oserror_;
  file_ = ex.file_;
  line_ = ex.line_;
  comment_ = ex.comment_;
}


//------------------------------------------------------------------------------
// ~CPS_BUAP_Loader_Exception
//------------------------------------------------------------------------------
 
 CPS_BUAP_Loader_Exception::~CPS_BUAP_Loader_Exception ()
{
}
 
//------------------------------------------------------------------------------
// errorCode
//------------------------------------------------------------------------------

CPS_BUAP_Loader_Exception::errorCode
CPS_BUAP_Loader_Exception::errCode () const
{
  return error_;
}

//------------------------------------------------------------------------------
// errorMessage
//------------------------------------------------------------------------------

const char *
CPS_BUAP_Loader_Exception::errorMessage () const
{
  return errorMessages[error_].errorText;
}

//------------------------------------------------------------------------------
// detailInfo
//------------------------------------------------------------------------------

const char*
CPS_BUAP_Loader_Exception::detailInfo () const
{
  return comment_.data();
}

//------------------------------------------------------------------------------
// operator<< 
//------------------------------------------------------------------------------

ostream& 
operator<< (ostream& s, const CPS_BUAP_Loader_Exception& ex)
{ 
  s.clear (0);

  s << "File: " << ex.file_ << endl;

  s << "Line: " << ex.line_ << endl;

  s << "Error: " << "(" << ex.error_ << ")" << endl;

  s << "ErrorMessage: " << errorMessages[ex.error_].errorText << endl;

  s << "Comment: " << ex.comment_ << endl;

  s << "OS Error: " << "(" << ex.oserror_ << ") " 
    << strerror(ex.oserror_) << endl;

  return s;
}

//------------------------------------------------------------------------------
//	Append character string to optional textual information  
//------------------------------------------------------------------------------

CPS_BUAP_Loader_Exception& 
CPS_BUAP_Loader_Exception::operator<< (const char* comment)
{
  comment_ += comment;
  return *this;
}


//------------------------------------------------------------------------------
//	Append long to optional textual information  
//------------------------------------------------------------------------------

CPS_BUAP_Loader_Exception& 
CPS_BUAP_Loader_Exception::operator<< (long comment)
{
  char buff [30];
  sprintf (buff, "%ld", comment);
  comment_ += buff; 
  return *this;
}


//------------------------------------------------------------------------------
//	Append istream to optional textual information  
//------------------------------------------------------------------------------

CPS_BUAP_Loader_Exception& 
CPS_BUAP_Loader_Exception::operator<< (istream& s)
{
  RWCString str;
  str.readString (s);
  comment_ += str;
  return *this;
}

