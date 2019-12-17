template<class v_t, class e_t>
bool GraphMat::CC<v_t, e_t>::reduce(v_t& scratch, const v_t& message) {
  // Reduce
  if(message < scratch) {
    scratch = message;
    return true;
  }
  return false;
}

template<class v_t, class e_t>
void GraphMat::CC<v_t, e_t>::process_edge(v_t& message, const e_t& edge, const v_t& vertex) {
  // Process Edge
  message = vertex;
}

template<class v_t, class e_t>
bool GraphMat::CC<v_t, e_t>::apply(const v_t& scratch, v_t& dram) {
  // Apply return true if successfully updated DRAM
  if(scratch != dram) {
    dram = scratch;
    return true;
  }
  return false;
}
