#include <colib.h>
#include <limits.h>
#include "iustring.h"

// -*- C++ -*-

// Copyright 2009 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
// or its licensors, as applicable.
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
// File: test-iustring.cc
// Purpose: testing iustring
// Responsible: remat
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de



using namespace colib;

int main(int argc,char **argv) {
    // -- test assignments --
    iucstring s1("Hello");
    iucstring s2("World");
    iucstring s3((int)6);

    TEST_ASSERT(s1.length() == 5);
    TEST_ASSERT(s2.length() == 5);
    TEST_ASSERT(s3.length() == 0);
    TEST_ASSERT(s3.empty());

    s3.assign(s2);
    TEST_ASSERT(s3.length() == 5);

    // -- test compare methods --
    TEST_ASSERT(s1.compare(s2) < 0);
    TEST_ASSERT(s1.compare(s1) == 0);
    TEST_ASSERT(s2.compare(s1) > 0);
    TEST_ASSERT(s2.compare(s3) == 0);

    TEST_ASSERT(s1 != s2);
    TEST_ASSERT(s1 == "Hello");
    TEST_ASSERT(s2 != "");
    TEST_ASSERT(s2 == s3);
    TEST_ASSERT("World" == s3);

    s3.push_back('!');
    TEST_ASSERT(s3.length() == 6);
    TEST_ASSERT(s3 == s2 + "!");
    TEST_ASSERT(s2.compare(s3) < 0);
    TEST_ASSERT(s3.compare(s2) > 0);

    // -- test element access --
    TEST_ASSERT(s1.at(0) == 'H');
    TEST_ASSERT(s2[2] == 'r');
    s3[5] = '?';
    TEST_ASSERT(s3 == "World?");

    // -- test modifiers --
    char test[6];
    s3.copy(test, 5);
    test[5] = '\0';
    TEST_ASSERT(strcmp(test, "World") == 0);

    s3.assign(s1);
    s3.push_back(' ');
    s3.append(s2);
    TEST_ASSERT(s3 == "Hello World");

    s3.insert(6, "little ");
    TEST_ASSERT(s3 == "Hello little World");
    s3.insert(0, s1);
    TEST_ASSERT(s3 == "HelloHello little World");
    s3.append("!");
    TEST_ASSERT(s3 == "HelloHello little World!");
    s3.erase(5, 5);
    TEST_ASSERT(s3 == "Hello little World!");
    s3.replace(6, 4, s1, 3);
    TEST_ASSERT(s3 == "Hello Helle World!");
    s3.erase(6);
    s3 += s2;
    TEST_ASSERT(s3 == "Hello World");

    iucstring s4;
    s4.append("abc");
    s4 = "123" + s4;
    s4 += 123;
    s4 += s4;
    TEST_ASSERT(s4 == "123abc123123abc123");
    s4.clear();
    s4 += "xyz";
    s4 = s4 + " " + 2.5;
    TEST_ASSERT(s4 == "xyz 2.500000");

    // -- test finds --
    TEST_ASSERT(s3.find(s1) == 0);
    TEST_ASSERT(s3.find(s1, 1) == iucstring::npos);
    TEST_ASSERT(s3.find(s1 + "-") == iucstring::npos);
    TEST_ASSERT(s3.find(s2) == 6);
    TEST_ASSERT(s3.find("ll", 0, 2) == 2);
    TEST_ASSERT(s3.find("ll", 3) == iucstring::npos);
    TEST_ASSERT(s3.rfind("l") == 9);
    TEST_ASSERT(s3.rfind("l", 8) == 3);
    TEST_ASSERT(s3.rfind("ll") == 2);

    // -- test regular expressions --
    TEST_ASSERT(re_search(s3, "^H.*d$") == 0);
    TEST_ASSERT(re_search(s3, "^H[^d]*d$") == 0);
    TEST_ASSERT(re_search(s3, "^H[^o]*d$") < 0);
    TEST_ASSERT(re_search(s3, "llo") == 2);
    TEST_ASSERT(re_search(s3, ". .") == 4);
    re_sub(s3, "e", "a");
    TEST_ASSERT(s3 == "Hallo World");
    re_gsub(s3, "o", "ooo");
    TEST_ASSERT(s3 == "Hallooo Wooorld");

    // -- test printing and scanning --
    sprintf(s3, "%s", "test");
    TEST_ASSERT(s3 == "test");
    sprintf_append(s3, " %d", 123);
    TEST_ASSERT(s3 == "test 123");
    char s[5];
    int d;
    scanf(s3, "%s %d", s, &d);
    TEST_ASSERT(strcmp(s, "test") == 0);
    TEST_ASSERT(d == 123);

    // -- test reading and writing --
    s3 = "Hello World";
    s4.clear();
    FILE* file = fopen("_test_123_", "w");
    fwrite(s3, file);
    fclose(file);
    file = fopen("_test_123_", "r");
    fread(s4, file);
    fclose(file);
    TEST_ASSERT(s3 == s4);

    // -- testing UTF-8 conversions --
    iustring<int> s8;
    s8.push_back('0'); // Digit Zero
    s8.push_back(0xE4); // Latin Small Letter A With Diaeresis
    s8.push_back(0xF6); // Latin Small Letter O With Diaeresis
    s8.push_back(0xFC); // Latin Small Letter U With Diaeresis
    s8.push_back(0x20AC); // Euro Sign
    s8.push_back(0xDF); // Latin Small Letter Sharp S (german)
    s8.push_back(0x2079); // Superscript Nine
    s8.push_back(0x1E83); // Latin Small Letter W With Acute
    s8.push_back(0x1EF3); // Latin Small Letter Y With Grave
    s8.push_back('?'); // Question Mark
    s8.push_back(0xFFFD); // Question Mark
    char t[256];
    int n;
    bytearray utf8;
    encodeUTF8(utf8, s8);
    file = fopen("__utf-8-test__", "w");
    n = fwrite(utf8.data, 1, utf8.length(), file);
    fclose(file);
    file = fopen("__utf-8-test__", "r");
    n = fread(t, 1, 255, file);
    fclose(file);
    iustring<int> s9;
    decodeUTF8(s9, t, n);
    TEST_ASSERT(s8.length() == s9.length());
    for(int i=0; i<s8.length(); i++) {
        TEST_ASSERT(s8[i] == s9[i]);
    }
    // -- testing UTF-16 conversions --
    bytearray utf16;
    encodeUTF16(utf16, s8);
    file = fopen("__utf-16-test__", "w");
    n = fwrite(utf16.data, 1, utf16.length(), file);
    fclose(file);
    file = fopen("__utf-16-test__", "r");
    n = fread(t, 1, 255, file);
    fclose(file);
    s9.clear();
    decodeUTF16(s9, t, n);
    TEST_ASSERT(s8.length() == s9.length());
    for(int i=0; i<s8.length(); i++) {
        TEST_ASSERT(s8[i] == s9[i]);
    }

    // -- test nustring conversion --
    nustring ns = "Hello World";
    s3.assign(ns);
    ns = "123456";
    TEST_ASSERT(ns == "123456");
    s3.toNustring(ns);
    TEST_ASSERT(ns == "Hello World");

    // -- tests for components.cc --
    iucstring prefix = "prefix";
    prefix += "_";
    iucstring entry = "prefix_key=value";
    TEST_ASSERT(!entry.compare(0, prefix.length(), prefix));
    int where = entry.find('=');
    TEST_ASSERT(where >= 0);
    entry.erase(where);
    TEST_ASSERT(entry.substr(prefix.length()) == "key");

    iucstring base = "filename.png";
    base.erase(base.length()-4);
    TEST_ASSERT(base == "filename");
	return 0;
}
