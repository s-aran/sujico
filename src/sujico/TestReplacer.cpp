#include "Header.h"
#include "Replacer.h"


#ifdef GTEST

class ReplacerTest : public ::testing::Test
{
protected:
  virtual void SetUp()
  {
    std::cout << "setup" << std::endl;
  }

  virtual void TearDown()
  {
    std::cout << "teardown" << std::endl;
  }
};


TEST_F(ReplacerTest, AddCallback)
{
  using namespace Piyo;

  std::string actKey, actValue, actFormat;
  auto init = [&]()
  {
    actKey = actValue = actFormat = "";
  };

  auto r = Replacer();
  r.add("test", [&](const std::string& k, const std::string& v, const std::string& f)
    {
      actKey = k;
      actValue = v;
      actFormat = f;
      return true;
    });
  r.add("EEEE", [&](const std::string& k, const std::string& v, const std::string& f)
    {
      actKey = k;
      actValue = v;
      actFormat = f;
      return false;
    });
  r.add("DDDD", [&](const std::string& k, const std::string& v, const std::string& f)
    {
      actKey = k;
      actValue = v;
      actFormat = f;
      return false;
    });

  
  {
    Replacer::TableMap m = { {"test", "hoge"} };

    init();
    ASSERT_TRUE(r.replace("piyo", m));
    ASSERT_EQ("test", actKey);
    ASSERT_EQ("hoge", actValue);
    ASSERT_EQ("piyo", actFormat);
  }

  {
    Replacer::TableMap m = { {"AAAA", "BBBB"} };

    init();
    ASSERT_TRUE(r.replace("fuga", m));
    ASSERT_EQ("", actKey);
    ASSERT_EQ("", actValue);
    ASSERT_EQ("", actFormat);
  }

  {
    Replacer::TableMap m = { {"DDDD", "EEEE"} };

    init();
    ASSERT_FALSE(r.replace("moge", m));
    ASSERT_EQ("DDDD", actKey);
    ASSERT_EQ("EEEE", actValue);
    ASSERT_EQ("moge", actFormat);
  }

  {
    Replacer::TableMap m =
    {
      {"AAAA", "BBBB"},
      {"CCCC", "DDDD"},
      {"test", "hoge"},
    };

    ASSERT_TRUE(r.replace("piyo", m));
  }
}


#endif  /* GTEST */

