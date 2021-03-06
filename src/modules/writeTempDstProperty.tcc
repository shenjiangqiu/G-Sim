/*
 * Andrew Smith
 *
 * WriteTempDstProperty
 *  This is the last stage in the processing phase, it writes the temp value
 *  back to memory and tells the ControlAtomicUpdate module a edge has finished.
 */

#include <cassert>

template<class v_t, class e_t>
SimObj::WriteTempDstProperty<v_t, e_t>::WriteTempDstProperty() {
  _scratchpad = NULL;
  _scratch_mem = NULL;
  _cau = NULL;
  _apply = NULL;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
}


template<class v_t, class e_t>
SimObj::WriteTempDstProperty<v_t, e_t>::WriteTempDstProperty(Memory* scratchpad, ControlAtomicUpdate<v_t, e_t>* cau, std::map<uint64_t, Utility::pipeline_data<v_t, e_t>>* scratch_mem, std::list<uint64_t>* apply) {
  assert(scratchpad != NULL);
  assert(cau != NULL);
  assert(scratch_mem != NULL);
  assert(apply != NULL);
  _apply = apply;
  _scratchpad = scratchpad;
  _cau = cau;
  _scratch_mem = scratch_mem;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
  _edges_written = 0;
}


template<class v_t, class e_t>
SimObj::WriteTempDstProperty<v_t, e_t>::~WriteTempDstProperty() {
  _scratchpad = NULL;
  _scratch_mem = NULL;
  _cau = NULL;
  _apply = NULL;
}


template<class v_t, class e_t>
void SimObj::WriteTempDstProperty<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
#ifdef MODULE_DEBUG
        std::cout << "Tick:" << _tick << " " << _name << " recieved: " << _data << "\n";
#endif
        _ready = false;
        _mem_flag = false;
        _scratchpad->write(0x01, &_mem_flag);
        _stall = STALL_MEM;
        next_state = OP_MEM_WAIT;
      }
      else {
        // Wait for upstream to send _edge
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
        _has_work = false;
      }
      break;
    }
    case OP_MEM_WAIT : {
      if(_mem_flag) {
        _scratch_mem->insert_or_assign(_data.vertex_dst_id, _data);
        _apply->push_back(_data.vertex_dst_id);
        _edges_written++;
        Utility::pipeline_data<v_t, e_t> temp_data = _cau->signal();
#ifdef DEBUG
        assert(temp_data.vertex_id == _data.vertex_id);
        assert(temp_data.edge_id == _data.edge_id);
        assert(temp_data.vertex_dst_id == _data.vertex_dst_id);
#endif
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
        _has_work = false;
      }
      else {
        next_state = OP_MEM_WAIT;
        _stall = STALL_MEM;
      }
      break;
    }
    default : {

    }
  }
#if 0
  if(_state != next_state) {
    std::cout << "[ " << __PRETTY_FUNCTION__ << " ] tick: " << _tick << "  state: " << _state_name[_state] << "  next_state: " << _state_name[next_state] << "\n";
  }
#endif
  _state = next_state;
  this->update_stats();
}


template<class v_t, class e_t>
void SimObj::WriteTempDstProperty<v_t, e_t>::print_stats(void) {
  sim_out.write("-------------------------------------------------------------------------------\n");
  sim_out.write("[ " + _name + " ]\n");
  sim_out.write("  Stalls:\n");
  sim_out.write("    STALL_CAN_ACCEPT: " + std::to_string(_stall_ticks[STALL_CAN_ACCEPT]) + " cycles\n");
  sim_out.write("    STALL_PROCESSING: " + std::to_string(_stall_ticks[STALL_PROCESSING]) + " cycles\n");
  sim_out.write("    STALL_PIPE:       " + std::to_string(_stall_ticks[STALL_PIPE]) + " cycles\n");
  sim_out.write("    STALL_MEM:        " + std::to_string(_stall_ticks[STALL_MEM]) + " cycles\n");
  sim_out.write("  Performance:\n");
  sim_out.write("    Edges:            " + std::to_string(_edges_written) + "\n");
  sim_out.write("    Cycles:           " + std::to_string(_tick) + "\n");
}


template<class v_t, class e_t>
void SimObj::WriteTempDstProperty<v_t, e_t>::print_stats_csv(void) {
  sim_out.write(_name + "," 
    + std::to_string(_stall_ticks[STALL_CAN_ACCEPT]) + ","
    + std::to_string(_stall_ticks[STALL_PROCESSING]) + ","
    + std::to_string(_stall_ticks[STALL_PIPE]) + ","
    + std::to_string(_stall_ticks[STALL_MEM]) + ","
    + std::to_string(_edges_written) + ","
    + std::to_string(_tick) + "\n");
}
