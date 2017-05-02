#ifndef S3D_UTILITIES_CONCURRENCY_TIMED_LOOP_H
#define S3D_UTILITIES_CONCURRENCY_TIMED_LOOP_H

#include "s3d/utilities/rule_of_five.h"

#include <chrono>

// todo :inherit rule_of_five_interface<TimedLoop>
class TimedLoop : rule_of_five_interface<TimedLoop> {
 public:
  class Client : rule_of_five_interface<Client> {
   public:
    virtual void callback() = 0;
  };

  virtual void start(Client* client, std::chrono::milliseconds loopDuration) = 0;
  virtual void stop() = 0;
};

#endif  // S3D_UTILITIES_CONCURRENCY_TIMED_LOOP_H
