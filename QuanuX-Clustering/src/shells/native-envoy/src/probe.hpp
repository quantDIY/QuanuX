#pragma once

struct ProbeResult {
  bool bone_ok;
  bool nerve_ok;
  bool soul_ok;
  int overall_exit_code; // 0 for ok, > 0 for error
};

class Probe {
public:
  ProbeResult execute(bool fix);

private:
  int check_bone(bool fix);
  int check_nerve(bool fix);
  int check_soul(bool fix);
};
