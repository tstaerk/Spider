/*
 parser.cpp - demonstration of a parser in C++
*/

#include "parser.h"
#include <kdebug.h>
#include <iostream>
#include <stdio.h>

using namespace std;

Parsert::Parsert()
{
}
  
void Parsert::setDocumentLocator(QXmlLocator* l)
{
  loc=l;
}

bool Parsert::startDocument()
{
  return true;
}
  
bool Parsert::startElement( const QString&, const QString&, const QString& qName, const QXmlAttributes& att )
{
  if (qName=="a") 
  {
    kDebug() << att.qName(0) << endl;
    kDebug() << att.value(0) << endl;
    if (att.qName(0)=="href") 
    {
      hrefs.append(att.value(0));
      cout << att.value(0).toStdString() << endl;
    }
  }
  return true;
}

bool Parsert::endElement( const QString&, const QString&, const QString& )
{
  return true;
}

bool Parsert::characters( const QString& )
{
  return true;
}
 
int Parsert::getbodystarty() { kDebug() << "Entering function"; return bodystarty; }

int Parsert::getbodystartx() { kDebug() << "Entering function"; return bodystartx; }

int Parsert::hrefcount() { kDebug() << "Entering function"; return hrefs.size(); }

QString Parsert::href(int i)
{
  return hrefs.takeAt(i);
}
