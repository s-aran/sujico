#include "Header.h"
#include "VCS.h"



Piyo::VCS::Mercurial::Mercurial()
{
  bool r = this->executeVcs();

  if (!r)
  {
    std::cerr << "internal error" << std::endl;
    return;
  }

  this->prepareCallbacks();
  r &= this->parse();
  if (!r)
  {
    std::cerr << "internal error" << std::endl;
    return;
  }
}


void Piyo::VCS::Mercurial::prepareCallbacks()
{
  using namespace NAMESPACE_REGEX;

  const auto common = [](const std::string& restr, const std::string& parsed) 
  {
    std::string r = "";

    regex re(restr);
    smatch sm;
    if (regex_search(parsed, sm, re))
    {
      r = sm[1];
    }
    
    return std::move(r);
  };

  using DateTime = struct
  {
    std::string dateTime;
    std::string timeDiff;
  };

  const auto datetime = [&](const std::string& parsed)
  {
    DateTime r = { "", "" };

    static constexpr auto reStr = R"(^(.*?) (.*?) ([0-9]+?) ([0-9]+?):([0-9]+?):([0-9]+?) ([0-9]+) ([+-])([0-9]{2})([0-9]{2})$)";
    regex re(reStr);

    std::string datetimeStr = common(R"(^date: +(.*)$)", parsed);
    
    smatch sm;
    regex_search(datetimeStr, sm, re);

    std::string dt;
    {
      std::stringstream ss;
      ss << sm[7] << "-" << sm[2] << "-" << sm[3] << " " << sm[4] << ":" << sm[5] << ":" << sm[6];
      dt = ss.str();
    }

    std::string diff;
    {
      std::stringstream ss;
      ss << sm[8] << sm[9] << ":" << sm[10] << ":00";
      diff = ss.str();
    }
    
    r.dateTime = dt;
    r.timeDiff = diff;

    return std::move(r);
  };

  const auto datetimeFormat = [](const DateTime& dt, bool isUtc, const std::string& format)
  {
    std::string r = "";

    auto t = boost::posix_time::time_from_string(dt.dateTime);
    auto td = boost::posix_time::duration_from_string(dt.timeDiff);

    auto* tf = new boost::posix_time::time_facet(format.c_str());

    std::stringstream ss;
    const auto tmp = ss.imbue(std::locale(std::locale::classic(), tf));
    ss << (isUtc ? t - td : t);
    r = ss.str();
    auto _ = ss.imbue(tmp);

    return std::move(r);
  };

  const auto dateFormat = [&](const DateTime& dt, bool isUtc, const std::string& format = "%Y/%m/%d")
  {
    return datetimeFormat(dt, isUtc, format);
  };

  const auto timeFormat = [&](const DateTime& dt, bool isUtc, const std::string& format = "%H:%M:%S")
  {
    return datetimeFormat(dt, isUtc, format);
  };


  this->addCallback("changeset", [&](const auto& key, const auto& parsed)
    {
      std::string r = common(R"(^changeset: +([0-9]+):[0-9a-f]+$)", parsed);
      return std::move(r);
    });

  this->addCallback("hash", [&](const auto& key, const auto& parsed)
    {
      std::string r = common(R"(^changeset: +[0-9]+:([0-9a-f]+)$)", parsed);
      return std::move(r);
    });

  this->addCallback("tag", [&](const auto& key, const auto& parsed)
    {
      std::string r = common(R"(^tag: +(.+?)$)", parsed);
      return std::move(r);
    });
  
  this->addCallback("user", [&](const auto& key, const auto& parsed)
    {
      std::string r = common(R"(^user: +(.+?)<(.*)>$)", parsed);
      return std::move(r);
    });

  this->addCallback("date", [&](const auto& key, const auto& parsed)
    {
      bool isUtc = false;
      DateTime dt = datetime(parsed);
      std::string r = dateFormat(dt, isUtc);
      return std::move(r);
    });

  this->addCallback("time", [&](const auto& key, const auto& parsed)
    {
      bool isUtc = false;
      DateTime dt = datetime(parsed);
      std::string r = timeFormat(dt, isUtc);
      return std::move(r);
    });

  this->addCallback("utcdate", [&](const auto& key, const auto& parsed)
    {
      bool isUtc = true;
      DateTime dt = datetime(parsed);
      std::string r = dateFormat(dt, isUtc);
      return std::move(r);
    });

  this->addCallback("utctime", [&](const auto& key, const auto& parsed)
    {
      bool isUtc = true;
      DateTime dt = datetime(parsed);
      std::string r = timeFormat(dt, isUtc);
      return std::move(r);
    });  
}


bool Piyo::VCS::Mercurial::executeVcs()
{
  bool r = true;

  static constexpr auto commandLine = "hg log -l 1 -b .";

  static constexpr auto envKey = "LANGUAGE";
  static constexpr auto envVal = "en_US.UTF-8";

  EnvironmentVariable::getInstance().set(envKey, envVal);
  r = this->execute(commandLine);
  EnvironmentVariable::getInstance().remove(envKey);

  return r;
}
