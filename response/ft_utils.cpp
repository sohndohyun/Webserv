#include "ft_utils.hpp"

std::string ft_makeGMT(char *tm_zone, time_t tv_sec)
{
	struct tm time;
	char buf[64];
	std::map<std::string, int> time_diff;
	time_diff["KST"] = 9 * 60 * 60;
	time_diff["JST"] = 9 * 60 * 60;
	time_diff["CTT"] = 8 * 60 * 60;
	time_diff["ECT"] = 1 * 60 * 60;
	time_diff["PST"] = -7 * 60 * 60;

	time_t gmt_time = tv_sec - time_diff[std::string(tm_zone)];
	strptime(std::to_string(gmt_time).c_str(), "%s", &time);
	strftime(buf, sizeof(buf), "%a, %d %b %G %T GMT", &time);
	return (buf);
}
