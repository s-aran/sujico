#include "Header.h"
#include "EnvironmentVariable.h"
#include "version.h"
#include "Executor.h"
#include "VCS.h"

#define NAMESPACE_REGEX boost

namespace Piyo
{
  constexpr auto ApplicationName = "sujico (suji command)";
  constexpr auto DefaultConfigFile = ".sujiconf.toml";

  // toml key
  static inline constexpr auto FormatKey = "format";

  // internal map key
  static inline constexpr auto DateTimeKey = "datetime";
  static inline constexpr auto TimeDiffKey = "timediff";

  enum class Verbose : uint8_t
  {
    VERBOSE_0 = 0,
    VERBOSE_1,
    VERBOSE_2,
  };

  static Verbose verbose = Verbose::VERBOSE_0;
  void debug(const std::string& msg)
  {
    if (verbose > Verbose::VERBOSE_0)
    {
      std::cerr << msg << std::endl;
    }
  }

  void debug2(const std::string& msg)
  {
    if (verbose > Verbose::VERBOSE_1)
    {
      std::cerr << msg << std::endl;
    }
  }

  namespace Placeholders
  {
    enum class VCS : uint8_t
    {
      NONE    = 0,

      SVN     = 1,
      GIT     = 2,
      HG      = 4,

      SVN_GIT = SVN | GIT,
      SVN_HG  = SVN | HG,
      GIT_HG  = GIT | HG,

      COMMON     = SVN | GIT | HG,
    };

    static inline const std::map<VCS, const std::string> Vcses =
    {
      {VCS::SVN , "svn" },
      {VCS::GIT , "git" },
      {VCS::HG  , "hg"  },
    };

    static inline const std::map<VCS, const std::vector<std::string>> Keys =
    {
      { VCS::HG    , { "changeset" }},
      { VCS::SVN_HG, { "revision", }},
      { VCS::GIT_HG, { "longhash", "hash" }},
      { VCS::COMMON, { "date", "time", "utcdate", "utctime", "commitlog", "tag", "branch", }},
    };
  };


inline bool isFileExists(const std::string& path)
{
  std::ifstream ifs(path, std::ios_base::binary);
  return ifs.good();
}

template <typename T>
inline bool hasKey(const std::string& key, const T& table)
{
  if constexpr (std::is_same<T, toml::value::table_type>::value)
  {
    const auto& itr = table.find(key);
    return table.cend() != itr;
  }
  else
  {
    const auto& itr = std::find(table, key);
    return std::cend(table) != itr;
  }
}

inline bool lazyCompare(const std::string& a, const std::string& b)
{
  const auto lca = boost::to_lower_copy(a);
  const auto lcb = boost::to_lower_copy(b);
  return lca == lcb;
}

const auto getGitLog(const std::string& path = "")
{
  auto vcs = VCS::Git();
  auto rr = vcs.getCaches();
  return std::move(rr);
}

const auto getHgLog(const std::string& path = "")
{
  auto vcs = VCS::Mercurial();
  auto rr = vcs.getCaches();
  return std::move(rr);
  
#if 0

  using namespace NAMESPACE_REGEX;
  std::map<std::string, std::string> r;

  static constexpr auto CommandLine = "hg log -l 1 -b .";
  std::string command = CommandLine;
  if (path != "")
  {
    command += " " + path;
  }

  static constexpr auto LanguageEnviron = "LANGUAGE";
  EnvironmentVariable::getInstance().set(LanguageEnviron, "en_US.UTF-8");
  const auto out = getStdout(command);
  EnvironmentVariable::getInstance().remove(LanguageEnviron);

  // std::cerr << ("hg:") << std::endl;
  // std::cerr << (out) << std::endl;

  static const regex reChangeset(R"(^changeset: +([0-9]+):([0-9a-f]+)$)");
  static const regex reTag(R"(^tag: +(.*)$)");
  static const regex reUser(R"(^user: +(.*?) <(.*)>$)");
  static const regex reDate(R"(^date: +(.*)$)");

  smatch smChangeset, smTag, smUser, smDate;
  if (regex_search(out, smChangeset, reChangeset))
  {
    r["changeset"] = smChangeset[1];
    r["hash"] = smChangeset[2];

    debug2("change set:");
    debug2(r["changeset"]);
    debug2("hash:");
    debug2(r["hash"]);
  }

  if (regex_search(out, smTag, reTag))
  {
    r["tag"] = smTag[1];
    debug2("tag:");
    debug2(r["tag"]);
  }

#if 0
  if (regex_search(out, smUser, reUser))
  {
    std::cout << "user: " << smUser[1] << " / " << smUser[2] << std::endl;
  }
#endif

  if (regex_search(out, smDate, reDate))
  {
    static constexpr auto reStr = R"(^(.*?) (.*?) ([0-9]+?) ([0-9]+?):([0-9]+?):([0-9]+?) ([0-9]+) ([+-])([0-9]{2})([0-9]{2})$)";
    const regex re(reStr);
    smatch sm;

    std::string s = smDate[1].str();
    if (regex_search(s, sm, re))
    {
      std::string std;
      {
        std::stringstream ss;
        ss << sm[7] << "-" << sm[2] << "-" << sm[3] << " " << sm[4] << ":" << sm[5] << ":" << sm[6];
        std = ss.str();
      }

      std::string stdiff;
      {
        std::stringstream ss;
        ss << sm[8] << sm[9] << ":" << sm[10] << ":00";
        stdiff = ss.str();
      }

      r[DateTimeKey] = std;
      r[TimeDiffKey] = stdiff;
      // std::cout << "----------" << std::endl;
      // std::cout << std << std::endl;
      // std::cout << stdiff << std::endl;
      // std::cout << r[TimeDiffKey] << std::endl;

      /*
      auto td = boost::posix_time::duration_from_string(stdiff);
      auto time = boost::posix_time::time_from_string(std);
      std::cout << time << "/" << sm[8] << " " << sm[9] << " " << sm[10] << std::endl;
      std::cout << td << " -> " << (time - td) << std::endl;
      */
    }
    else
    {
      debug("date format not match");
      debug(out);
    }
  }
  else
  {
    debug("date not match");
    debug(out);
  }

  return std::move(r);
#endif
}

const std::string formatValue(const std::string& key, const std::map<std::string, std::string> table, const std::string& format)
{
  std::string r;

  std::string value;

  auto itr = table.find(key);
  bool found = itr != std::end(table);
  if (found)
  {
    value = (*itr).second;
  }

  if (found)
  {
    r = value;
  }

  return std::move(r);
}

const std::string formatValue(const std::string& key, const toml::value& value, const std::string& format)
{
  std::string r;

  auto type = value.type();
  if (type == toml::value_t::integer)
  {
    std::string fmt = "%d";
    if (format != "")
    {
      fmt = "%%%dd";
      fmt = (boost::format(fmt) % format).str();
    }

    int v = static_cast<int>(value.as_integer());
    r = (boost::format(fmt) % v).str();
  }
  else if (type == toml::value_t::string)
  {
    std::string fmt = "%s";
    if (format != "")
    {
      fmt = "%%%ds";
      fmt = (boost::format(fmt) % format).str();
    }

    std::string v = value.as_string();
    r = (boost::format(fmt) % v).str();
  }

  return std::move(r);
}

const std::string replace(const std::string& format, const toml::table& table)
{
  using namespace NAMESPACE_REGEX;

  const std::string cr = format;
  std::string r = format;

  static constexpr auto  restrBegin  = R"(\$\{)";
  static constexpr auto  restrEnd    = R"(\})";
  static const     auto  restr       = std::string(restrBegin) + R"((.*?))" + std::string(restrEnd);
  static const     regex re(restr);

  static constexpr auto  frestr = R"((?:(.*?):)(.*?))";
  static const     regex fre(frestr);

  for (sregex_iterator bitr(std::cbegin(cr), std::cend(cr), re), bend; bitr != bend; ++bitr)
  {
    auto&& m = *bitr;
    {
      bool found = false;
      std::string sss = m[1];

      // std::vector<std::string> formats;
      // boost::algorithm::split(formats, sss, boost::is_any_of(":"));

      std::string value;

      // format
      smatch sm;
      if (regex_match(sss, sm, fre))
      {
        std::string f = sm[1];
        std::string k = sm[2];
        
        if (f == Placeholders::Vcses.at(Placeholders::VCS::SVN) ||
            f == Placeholders::Vcses.at(Placeholders::VCS::GIT) ||
            f == Placeholders::Vcses.at(Placeholders::VCS::HG ))
        {
          auto vcs = Placeholders::VCS::NONE;
          for (auto& kv : Placeholders::Keys)
          {
            vcs = kv.first;
            auto& l = kv.second; // Placeholders::Keys.at(Placeholders::VCS::COMMON);
            auto itr = std::find(std::begin(l), std::end(l), k);
            found = itr != std::end(l);
            if (found)
            {
              break;
            }
          }

          switch (vcs)
          {
          case Placeholders::VCS::SVN_GIT:
            break;
          case Placeholders::VCS::SVN_HG:
            if (k == "revision")
            {
              k = "changeset";
            }
            break;
          case Placeholders::VCS::GIT_HG:
            break;
          default:
            break;
          }

          smatch sm2;
          auto f2 = (regex_search(k, sm2, fre) ? sm2[1].str() : "");
          
          VCS::CacheMap vcsmap;
          if (f == Placeholders::Vcses.at(Placeholders::VCS::SVN))
          {
            // 
          }
          else if (f == Placeholders::Vcses.at(Placeholders::VCS::GIT))
          {
            vcsmap = getGitLog();
          }
          else if (f == Placeholders::Vcses.at(Placeholders::VCS::HG))
          {
            vcsmap = getHgLog();
          }

          if (found)
          {
            value = formatValue(k, vcsmap, f2);
          }
        }
        else
        {
          auto itr = table.find(k);
          found = itr != std::end(table);
          if (found)
          {
            auto v = (*itr).second;
            value = formatValue(k, v, f);
          }
        }
      }
      else
      {
        const auto& itr = table.find(sss);
        found = itr != std::end(table);
        if (found)
        {
          value = formatValue(sss, (*itr).second, "");
        }
      }

      // replace
      if (found)
      {
        const std::string restr2 = std::string(restrBegin) + sss + std::string(restrEnd);
        r = regex_replace(r, regex(restr2), value, regex_constants::format_first_only);
      }
    }
  }

  return std::move(r);

  /* smatch m;
  auto sr = regex_search(r, m, re);
  std::cout << restr << std::endl;
  if (sr)
  {
    std::cout << "*** " << std::endl;
    for (auto& s : m)
    {
      std::cout << "* " << s.str() << std::endl;
    }
  }

  for (auto& k : table)
  {
    std::cout << k.first;
    std::cout << ": ";
    std::cout << k.second;
    std::cout << std::endl;


    std::string value;
    switch (k.second.type())
    {
    case toml::value_t::integer:
    {
      std::stringstream ss;
      ss << k.second.as_integer();
      value = ss.str();
      break;
    }
    case toml::value_t::string:
    {
      value = k.second.as_string();
      break;
    }
    default:
    {
    }
    }

    const regex bracket(re);
    r = regex_replace(r, bracket, value);
    std::cout << "----> " << r << std::endl;
  }

  return std::move(r);
    */
}

bool parseConfigure(const std::string& path, toml::value::table_type& result)
{
  if (!isFileExists(path))
  {
    std::cerr << "file read error: " << path << std::endl;
    return false;
  }

  try
  {
    auto conf = toml::parse(path);
    result = conf.as_table();
  }
  catch (const std::exception& e)
  {
    std::cerr << "file parse error: " << e.what() << std::endl;
    return false;
  }
  
  return true;
}

bool increment(toml::value::table_type& table, const std::vector<std::string> list)
{
  using namespace NAMESPACE_REGEX;
  static const regex reNum(R"([0-9]+)");
  static const regex reChar(R"([a-zA-Z])");

  bool result = true;

  for (auto& k : list)
  {
    // std::cout << k << ": " << (hasKey(k, table) ? "exist" : "not exist") << std::endl;
    if (!hasKey(k, table))
    {
      std::cerr << k << " is undefined" << std::endl;
      result = false;
      break;
    }
    else
    {
      auto value = table[k];
      auto type = value.type();

      std::string cur;
      auto out = cur;
      
      switch (type)
      {
      case toml::value_t::integer:
      {
        auto v = value.as_integer();
        cur = boost::lexical_cast<std::string>(v);
        break;
      }
      case toml::value_t::string:
      {
        cur = value.as_string().str;
        break;
      }
      default:
      {
        break;
      }
      }

      if (regex_match(cur, reNum))
      {
        try
        {
          auto v = boost::lexical_cast<uint64_t>(cur);
          if (v >= static_cast<uint64_t>(-1))
          {
            throw boost::bad_lexical_cast();
          }
          v++;
          out = boost::lexical_cast<std::string>(v);
        }
        catch ([[maybe_unused]] const boost::bad_lexical_cast & e)
        {
          std::cerr << "cannot increment: " << cur << std::endl;
          result = false;
          break;
        }
      }
      else if (regex_match(cur, reChar))
      {
        try
        {
          auto v = boost::lexical_cast<char>(cur);
          if (v == 'z' || v == 'Z')
          {
            throw boost::bad_lexical_cast();
          }
          v++;
          out = std::string({ v });
        }
        catch ([[maybe_unused]] const boost::bad_lexical_cast &  e)
        {
          std::cerr << "cannot increment: " << cur << std::endl;
          result = false;
          break;
        }
      }
      else
      {
        std::cerr << "cannot increment: " << value << std::endl;
        result = false;
        break;
      }

      table[k] = out;
    }
  }

  return result;
}

bool setValue(toml::value::table_type& table, const std::map<std::string, std::string>& up)
{
  bool result = true;

  for (auto& itr : up)
  {
    const auto& k = itr.first;
    const auto& v = itr.second;

    if (!hasKey(k, table))
    {
      std::cerr << k << " is undefined" << std::endl;
      result = false;
      break;
    }

    const auto& tableValue = table[k];
    const auto type = tableValue.type();

    switch (type)
    {
    case toml::value_t::integer:
    {
      try
      {
        const auto newValue = boost::lexical_cast<uint64_t>(v);
        table[k] = newValue;
      }
      catch ([[maybe_unused]] const boost::bad_lexical_cast & e)
      {
        std::cerr << v << " is wrong type" << std::endl;
        result = false;
        goto SETVALUE_RETURN;
      }
      break;
    }
    case toml::value_t::string:
    {
      table[k] = v;
      break;
    }
    default:
    {
      break;
    }
    }
  }

SETVALUE_RETURN:
  return result;
}

bool update(toml::value::table_type& table, bool print=false)
{
  auto format = table[FormatKey].as_string();
  if (print)
  {
    std::cout << replace(format, table) << std::endl;
  }
  return true;
}

bool writeFile(const toml::value::table_type& table, const std::string& writePath)
{
  toml::value v = table;

  std::ofstream fs;
  fs.open(writePath, std::ios::out | std::ios::binary);
  if (!fs.is_open())
  {
    return false;
  }

  fs << v;
  fs.close();

  return !fs.bad();
}

void showHelp(boost::program_options::options_description& d)
{
  std::cout << d << std::endl;
}

void showVersion()
{
  std::cout << Version::get(ApplicationName) << std::endl;
}

void showLicenses()
{
  static constexpr auto toml11License = R"""(
toml11
https://github.com/ToruNiina/toml11

The MIT License (MIT)

Copyright (c) 2017 Toru Niina

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.)""";
  
  static constexpr auto boostLicense = R"""(
Boost
https://www.boost.org/

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.)""";
  
  std::cout << toml11License << std::endl << std::endl << boostLicense << std::endl;
}

};

int main(int argc, char* argv[])
{
#ifdef GTEST
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
#else
  using namespace NAMESPACE_REGEX;
  using namespace Piyo;

  // std::cout << Version::get(ApplicationName) << std::endl;
  // std::cout << EnvVar::get("TMP") << std::endl;
  // 
  // 
  // std::cout << (EnvVar::isExists("hogeho22ge") ? "exists" : "NOT exists") << std::endl;
  // EnvVar::set("hogehoge", "piyopiyo");
  // std::cout << EnvVar::get("hogehoge") << std::endl;
  // EnvVar::remove("hogehoge");
  // 
  // std::cout << (EnvVar::isExists("hogehoge") ? "exists" : "NOT exists") << std::endl;
  // 
  // auto out = getStdout("hg");

  std::string confFile = DefaultConfigFile;
  bool isFileOut = false;
  std::string outFile = "";

  bool print = true;

  std::vector<std::string> incrementList;
  std::map<std::string, std::string> setTable;

  bool result = true;

  {
    using namespace boost::program_options;

    std::stringstream sstitle;
    sstitle << ApplicationName << " options";
    options_description desc(sstitle.str());
    variables_map vm;

    auto setCallback = [&](auto& kv)
    {
      regex re(R"(^ *(.+?) *= *(.*?)?? *$)");
      smatch sm;
      if (!regex_search(kv, sm, re))
      {
        std::stringstream ss;
        ss << R"(")" << kv << R"(")" << " is unrecognized expression";
        throw boost::program_options::error(ss.str());
        return false;
      }

      bool hasKey = sm.length() > 1;
      std::string k = sm[1].str();
      
      if (hasKey)
      {
        setTable[k] = sm[2].str();
      }

      return true;
    };

    desc.add_options()
      ("increment,i"  , value<std::string>()->value_name("KEY")->notifier([&](auto& k)
        {
          incrementList.emplace_back(std::move(k));
        }), "increments the value of specified KEY")
        ("set,s"      , value<std::string>()->value_name("KEY=VALUE")->notifier(setCallback), "sets VALUE to specified KEY")
        ("file,f"     , value<std::string>()->value_name("FILE")->default_value(DefaultConfigFile), "specify config file")
        ("write,w"    , value<std::string>()->value_name("FILE")->implicit_value(""), "write to FILE. if FILE is empty, overwrite current config")
        // ("print,p"    , "print processed text")
        ("help,h"     , "show help")
        ("license,l"  , "show licenses")
        ("version,v"  , "show version")
        ("verbose"    , "verbose mode")
        ("VERBOSE"    , "more more verbose mode")
        ;

    try
    {
      command_line_parser parser(argc, argv);
      parser.options(desc);
      parsed_options parsed = parser.run();
      store(parsed, vm);
      notify(vm);
    }
    catch (const boost::program_options::error& e)
    {
      std::cerr << e.what() << std::endl;
      showHelp(desc);
      return EXIT_FAILURE;
    }
    catch (const std::exception & e)
    {
      std::cerr << e.what() << std::endl;
      return EXIT_FAILURE;
    }
    catch (...)
    {
      std::cerr << "fatal error" << std::endl;
      return EXIT_FAILURE;
    }


    if (vm.count("help"))
    {
      showHelp(desc);
      return EXIT_SUCCESS;
    }

    if (vm.count("version"))
    {
      showVersion();
      return EXIT_SUCCESS;
    }

    if (vm.count("license"))
    {
      showLicenses();
      return EXIT_SUCCESS;
    }

    if (vm.count("file"))
    {
      confFile = vm["file"].as<std::string>();
    }

    if (vm.count("write"))
    {
      const auto& arg = vm["write"].as<std::string>();
      isFileOut = true;

      outFile = confFile;
      if (arg.length() > 0)
      {
        outFile = arg;
      }
    }

    toml::value::table_type table;
    if (!parseConfigure(confFile, table))
    {
      std::cerr << "toml parse error" << std::endl;
      return EXIT_FAILURE;
    }

    if (vm.count("increment"))
    {
      result &= increment(table, incrementList);
    }

    if (vm.count("set"))
    {
      result &= setValue(table, setTable);
    }

    if (vm.count("print"))
    {
      print = true;
    }

    if (vm.count("verbose"))
    {
      verbose = Verbose::VERBOSE_1;
    }
    else if (vm.count("VERBOSE"))
    {
      verbose = Verbose::VERBOSE_2;
    }

    if (result)
    {
      result &= update(table, print);
      if (isFileOut)
      {
        result &= writeFile(table, outFile);
      }
    }

  }

  // std::cout << "hg:" << std::endl << out << std::endl;

  return (result ? EXIT_SUCCESS : EXIT_FAILURE);
#endif  /* GTEST */
}
