#pragma once

struct Version
{
  static inline constexpr auto Major            = "0";
  static inline constexpr auto Minor            = "00";
  static inline constexpr auto Build            = "240";
  static inline constexpr auto Milestone        = "beta";
  static inline constexpr auto MilestoneVersion = "1a";
  static inline constexpr auto VcsRevision      = "25";
  static inline constexpr auto VcsHash          = "adc271338690";
  static inline constexpr auto datetime         = "2019/12/15 13:58:59";
  static inline constexpr auto tag              = "tip";
  static inline constexpr auto VersionString    = "0.00.240 beta-1a (25:adc271338690)";

  static inline auto get(const std::string& pre = "", const std::string& post = "", const std::string& versionString = "version: ")
  {
    static std::stringstream ss;
    if (pre != "") ss << pre << " ";
    ss << versionString << VersionString;
    if (post != "") ss << " " << post;
    ss << std::endl;
    return ss.str();
  }
};

