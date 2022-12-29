#ifndef EXTERNALCALL_H
#define EXTERNALCALL_H

#include <string>

void scp(const std::string& from, const std::string& to);
void ssh_command(const std::string& host, const std::string& command);

#endif // EXTERNALCALL_H
