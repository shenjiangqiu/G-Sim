/*
 * Andrew Smith
 *
 * Control Atomic Update:
 *  The control atomic update module is part of the atomic update sequence of the
 *  graphicionado pipeline.
 *
 */

#ifndef CONTROLATOMICUPDATE_H
#define CONTROLATOMICUPDATE_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>
#include <list>

#include "module.h"
#include "memory.h"

#include "readGraph.h"

namespace SimObj {

template<class v_t, class e_t>
class ControlAtomicUpdate : public Module<v_t, e_t> {
private:
  enum op_t {
    OP_WAIT,
    OP_STALL,
    OP_NUM_OPS
  };

#ifdef DEBUG
  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_STALL"},
    {2, "OP_NUM_OPS"}};
#endif

  using Module<v_t, e_t>::_tick;
  using Module<v_t, e_t>::_ready;
  using Module<v_t, e_t>::_stall;
  using Module<v_t, e_t>::_next;
  using Module<v_t, e_t>::_data;
  using Module<v_t, e_t>::_name;
  using Module<v_t, e_t>::_has_work;

  op_t _state;
  bool _op_complete;
  std::list<Utility::pipeline_data<v_t, e_t>> _nodes;

  bool dependency();

public:
  ControlAtomicUpdate();
  ~ControlAtomicUpdate();

  void tick(void);
  Utility::pipeline_data<v_t, e_t> signal();
  void debug(void);
};

} // namespace SimObj

#include "controlAtomicUpdate.tcc"

#endif
