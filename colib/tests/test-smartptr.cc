#include "colib.h"
// -*- C++ -*-

// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
// or its licensors, as applicable.
// Copyright 1995-2005 Thomas M. Breuel
//
// You may not use this file except under the terms of the accompanying license.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you
// may not use this file except in compliance with the License. You may
// obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Project: iulib -- image understanding library
// File: test-smartptr.cc
// Purpose: test smart pointers
// Responsible: syed atif mehdi
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

/// \file test-smartptr.h
/// \brief test Smart pointers


using namespace colib;



/*
* A simple class for testing
*/
struct X
{
  public:
    X() : m(0) {}
    X(int i) : m(i) {}
    ~X() {}
    int m;
    int get_m() { return m; }
    void set_m(int i) { m =i;}
};


//Function taking autoref as a reference parameter

template<typename T>
void parameter_fun(autoref<T>& sp){
  TEST_OR_DIE(sp.operator->() != 0);
}

//Function returning autoref

template<typename T>
autoref<T> return_fun(){
  autoref<T> local;
  local->set_m(244);
  return local;
}

template<typename T>
void parameter_fun(autodel<T>& sp){
  TEST_OR_DIE(sp.operator->() != 0);
}

//Function returning autoref

/*template<typename T>
autodel<T> return_fun_del(){
  autodel<T> local(new T(244));
  
  return local;
}*/


void fun_autoref(){
  
  autoref<X> spx1;
  (*spx1).m=1;

  TEST_OR_DIE((*spx1).m == 1);
  (*spx1).m = 1147;
  TEST_OR_DIE((*spx1).m == 1147);
  TEST_OR_DIE(spx1->m == 1147);
  TEST_OR_DIE(spx1->get_m() == 1147);
  
  autoref<X> spx2;
  spx2=spx1;
  TEST_OR_DIE(spx2.operator->() != 0);
  TEST_OR_DIE(spx1->get_m() == 0);
  TEST_OR_DIE(spx2->get_m() == 1147);
  
  
  spx1 = spx2;
  
  TEST_OR_DIE(spx1.operator->() != 0);
  TEST_OR_DIE(spx1->get_m() == 1147);
  
  autoref<X> sp;
  sp->m=241;
  sp = sp;
  TEST_OR_DIE(sp->get_m() == 241);
  TEST_OR_DIE(sp.operator->() != 0);

  
  autoref<X> spf3;
  spf3->set_m(243);
  parameter_fun(spf3);
  TEST_OR_DIE(spf3.operator->() != 0);
  TEST_OR_DIE(spf3->get_m() == 243);
  
#if 0
  // no idea what this is supposed to test
  // autorefs can't be returned
  autoref<X> spf4;
  spf4=return_fun<X>();
  TEST_OR_DIE(spf4.operator->() != 0);
  TEST_OR_DIE(spf4->get_m() == 244);
#endif

}

void fun_autodel(){

  autodel<X> spx1(new X(1));

  TEST_OR_DIE( (*spx1).m ==1);
  (*spx1).m = 1147;
  TEST_OR_DIE( (*spx1).m ==1147);

  TEST_OR_DIE( spx1->m ==1147);
  TEST_OR_DIE( spx1->get_m() ==1147);
  
  autodel<X> spx2;
  spx2 = spx1;
  TEST_OR_DIE(spx2.operator->() != 0);
  TEST_OR_DIE(spx2->get_m() == 1147);
  
  spx1 = spx2;
  TEST_OR_DIE(spx1.operator->() != 0);
  try {
      spx2.operator->();
      TEST_OR_DIE(!"operator->() should have thrown an exception");
  }
  catch(const char *err) {
  }
  TEST_OR_DIE(spx1->get_m() == 1147);

  TEST_OR_DIE (spx1);
  TEST_OR_DIE(spx2 == 0);
  
  TEST_OR_DIE (!spx1 == false);
  TEST_OR_DIE (!spx2);
  
  TEST_OR_DIE (spx1 != 0);
  TEST_OR_DIE (spx1 == 1); // using bool() ...
  TEST_OR_DIE (0 != spx1);
  TEST_OR_DIE (0 == spx2);

  TEST_OR_DIE(spx1 == spx1);
  TEST_OR_DIE (spx1 != spx2);


  autodel<X> sp(new X(241));
  sp = sp;
  TEST_OR_DIE(sp != 0);

  autodel<X> spf3(new X(243));

  parameter_fun(spf3);
  TEST_OR_DIE(spf3.operator->() != 0);
  TEST_OR_DIE(spf3->get_m() == 243);

  /*autodel<X> spf4 = return_fun_del<X>();    
  TEST_OR_DIE (spf4);
  TEST_OR_DIE(spf4->get_m() == 244);*/
}

int main(){
  fun_autoref();
  fun_autodel();
  return 0;
}

