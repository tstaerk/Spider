/*
spider.cpp

This program searches an html file for all references and follows them.

(c) 2006-07-02 by Thorsten Staerk

build and run it like this:
 cmake . && make -j8 && ./spider
*/


#include <qstring.h>
#include <QXmlInputSource>
#include <QXmlLocator>
#include <qfile.h>
#include "parser.h"
#include "myqxmlerrorhandler.h"
#include <kdebug.h>
#include <kurl.h>
#include <tidy.h>
#include <buffio.h>
#include <stdio.h>
#include <errno.h>

void parsefile(QString file, int depth)
{
  kDebug() << "Entering function";
  static int fileid=1;
  static QString hostname=QString();
  KUrl kurl(file);
  kDebug() << kurl.host() << endl;
  if (depth==0)
  {
    hostname=kurl.host();
  }
  if (kurl.host()==QString()) 
  {
    QString oldfile=file;
    QString oldhost=hostname;
    file=oldhost.append(oldfile);
  }
  kDebug() << "hostname is " << hostname << endl;
  kDebug() << "file is " << file << endl;
  kDebug() << endl;
  fileid++;
  char s[80]="";
  sprintf(s,"file%d",fileid);
  QString savefilename=QString(s);
  if (depth<2)
  {
    Parsert* handler=new Parsert();
    QString cmd=QString("wget -O ").append(savefilename).append(QString(" ")).append(file).append(QString(" 2>/dev/null"));
    kDebug() << cmd << endl;
    // Download if the parameter is no local file.
    if ( !hostname.isEmpty() ) system(cmd.toAscii());
    QFile* htmlfile=new QFile(savefilename);
    kDebug() << "working with " << htmlfile->fileName();
    QXmlInputSource* source=new QXmlInputSource(htmlfile);
    QXmlSimpleReader reader;
  
    myqxmlerrorhandler* er=new myqxmlerrorhandler();
    reader.setContentHandler( handler );
    reader.setErrorHandler( er );
    reader.parse( source );
    for (int i=0; i<handler->hrefcount(); i++) 
    {
      kDebug() << "inspecting next trunk";
      QString descent=handler->href(i);
      kDebug() << descent << endl;
      parsefile(descent, depth+1);
    }
  }
}

QString tidyfile()
// convert the html file file1 into the xhtml file file2.
{
  kDebug() << "Entering function";
  // the following code is (c) Charles Reitzel and Dave Raggett, see the package tidy
  char* input = "<body lang=DE link=blue vlink=purple bgcolor=#eeeeff>";
  QFile inputfile("file1");
  if (inputfile.open(QIODevice::ReadOnly))
  {
    qint64 howlong=inputfile.bytesAvailable();
    input=(char*)malloc(howlong);
    kDebug() << "reading " << howlong;
    inputfile.read(input, howlong);
    TidyBuffer output = {0};
    TidyBuffer errbuf = {0};
    int rc = -1;
    Bool ok;

    TidyDoc tdoc = tidyCreate();                             // Initialize "document"
    printf( "Tidying:\t\%s\\n", input );

    ok = tidyOptSetBool( tdoc, TidyXhtmlOut, yes );          // Convert to XHTML
    if ( ok ) rc = tidySetErrorBuffer( tdoc, &errbuf );      // Capture diagnostics
    if ( rc >= 0 ) rc = tidyParseString( tdoc, input );      // Parse the input
    if ( rc >= 0 ) rc = tidyCleanAndRepair( tdoc );          // Tidy it up!
    if ( rc >= 0 ) rc = tidyRunDiagnostics( tdoc );          // Kvetch
    if ( rc > 1 )                                            // If error, force output.
      rc = ( tidyOptSetBool(tdoc, TidyForceOutput, yes) ? rc : -1 );
    if ( rc >= 0 ) rc = tidySaveBuffer( tdoc, &output );     // Pretty Print
    if ( rc >= 0 )
    {
      if ( rc > 0 )
        printf( "\\nDiagnostics:\\n\\n\%s", errbuf.bp );
      printf( "\\nAnd here is the result:\\n\\n\%s", output.bp );
      char* outputstring; // content of the outputfile

      // find out length of outputstring
      int length=0; // length of outputstring
      byte* string=output.bp;
      while (*string)
      {
        kDebug() << "byte is " << string << " value " << *string;
        string++;
        length++;
      }
  
      kDebug() << "allocating memory " << length;
      outputstring=(char*)malloc(length);
      kDebug() << "printing output string";
      snprintf(outputstring,length,"%s",output.bp);
      kDebug() << "outputstring is " << outputstring;
      kDebug() << "opening output file";
      QFile outputfile("file2");
      outputfile.open(QIODevice::WriteOnly);
      outputfile.write(outputstring,length);
    }
    else
      printf( "A severe error (\%d) occurred.\\n", rc );

    tidyBufFree( &output );
    tidyBufFree( &errbuf );
    tidyRelease( tdoc );
  } // if file1 existed
  else kDebug() << "file1 did not exist, could not convert to xhtml";
}

int main(int argc, char *argv[])
{
  tidyfile(); 
  parsefile(argv[1],0);
}