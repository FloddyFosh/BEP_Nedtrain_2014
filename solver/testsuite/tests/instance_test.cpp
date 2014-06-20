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


#define useRandom false

class InstanceTest : public ::testing::Test {
  protected:    
      ostringstream buffer;
      streambuf *sbuf;

      void parseFile(const char* filepath){
          FILE *myfile = fopen(filepath, "r");
          ASSERT_TRUE(myfile);
          // set lex to read from it instead of defaulting to STDIN:
          yyin = myfile;
          ASSERT_FALSE(yyparse());
      }

      virtual void SetUp() {
          tmsp = new tmsp_t;
          //Redirect stdout to buffer
          sbuf = cout.rdbuf();
          cout.rdbuf(buffer.rdbuf());
      }
      virtual void TearDown() {
          delete tmsp;
          //Redirect stdout back to itself
          cout.rdbuf(sbuf);
      }
};

TEST_F(InstanceTest, SmallInstance) {
    ADD_FAILURE();
    return;
    // TODO
    parseFile("../../instances/small.instance");
    solve(0,0);
    ifstream in("tests/ExpectedSolutions/small.txt", ios::in | ios::binary);
    std::string s((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    cdebug("3\n");
    size_t index = 0;
    while (true) {
         index = s.find("\\n", index);
         if (index == string::npos) break;
         s.replace(index, 2, "\n");
         index += 2;
    }
    ASSERT_EQ(s,buffer.str());
}
