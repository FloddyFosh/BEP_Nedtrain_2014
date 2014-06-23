#include  <gtest/gtest.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <fstream>

#include "../../src/timing.h"
#include "../../src/output.h"
#include "../../src/tmsp.h"
#include "../../src/solve.h"
#include "../../src/token.h"
#include "../../src/grammar.tab.hpp"

#undef useRandom
#define useRandom false

class InstanceTest : public ::testing::Test {
  protected:    
      ostringstream buffer;
      ostringstream ebuffer;
      streambuf* sbuf;
      streambuf* ebuf;

      void parseFile(const char* filepath){
          FILE *myfile = fopen(filepath, "r");
          ASSERT_TRUE(myfile);
          // set lex to read from it instead of defaulting to STDIN:
          yyin = myfile;
          ASSERT_FALSE(yyparse());
      }

      virtual void SetUp() {
          clear_tmsp();

          //Redirect stdout to buffer
          sbuf = cout.rdbuf();
          ebuf = cerr.rdbuf();
          cout.rdbuf(buffer.rdbuf());
          cerr.rdbuf(ebuffer.rdbuf());
      }
      virtual void TearDown() {
          delete tmsp;

          //Redirect stdout back to itself
          cout.rdbuf(sbuf);
          cerr.rdbuf(ebuf);
      }
};

TEST_F(InstanceTest, SmallInstance) {
    parseFile("../../instances/small.instance");
    tmsp_t* b = tmsp;
    solve(0,0);
    ifstream in("tests/ExpectedSolutions/small.txt", ios::in | ios::binary);
    string s((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    ASSERT_EQ(s,buffer.str());
}
