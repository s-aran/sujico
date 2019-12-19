#pragma once

struct Version
{
  static inline constexpr auto Major            = "0";
  static inline constexpr auto Minor            = "00";
  static inline constexpr auto Build            = "262";
  static inline constexpr auto Milestone        = "beta";
  static inline constexpr auto MilestoneVersion = "2a";
  static inline constexpr auto VcsRevision      = "27";
  static inline constexpr auto VcsHash          = "78b3e06947e3";
  static inline constexpr auto datetime         = "2019/12/16 23:55:17";
  static inline constexpr auto tag              = "tip";
  static inline constexpr auto VersionString    = "0.00.262 beta-2a (27:78b3e06947e3)";

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

