#pragma once
#include <atomic>
//
// INVARIANT: w_deleted_count <= w_deleting_count <= w_cached_read_count <= shared_read_count <= r_reading_count <= r_cached_write_count <= shared_write_count <= w_writing_count <= w_deleted_count + MAX_SIZE
//
// INVARIANT:      w_writing_count      - w_deleted_count     <= MAX_SIZE
// =========>      w_writing_count      - w_cached_read_count <= MAX_SIZE
// =========>      shared_write_count   - w_cached_read_count <= MAX_SIZE
// =========>      shared_write_count   - shared_read_count   <= MAX_SIZE
//
//
// INVARIANT: 0 <= r_cached_write_count - r_reading_count
// =========> 0 <= r_cached_write_count - shared_read_count
// =========> 0 <= shared_write_count   - shared_read_count
//
//
// THEOREM: =========> 0 <= shared_write_count   - shared_read_count   <= MAX_SIZE
//





//
//
// Channel/Queue:
//
// Shared between Producer and Consumer
//
struct channel_t{
  public:
  
    //insert your code here
    std::atomic<int> shared_read_count;
    std::atomic<int> shared_write_count;

  public:

    //
    // Intialize
    //
    channel_t(){

      // insert your code here
      shared_read_count = 0;
      shared_write_count = 0;

    }
};


//
// Producer's (not shared)
//
struct writeport_t{
public:
    //insert your code here
    int w_deleted_count;
    int w_deleting_count;
    int w_cached_read_count;
    int w_writing_count;
    int buf_size;

public:

  //
  // Intialize
  //
  writeport_t(size_t tsize)
  {
    //insert code here
    w_deleted_count = 0;
    w_deleting_count = 0;
    w_cached_read_count = 0;
    w_writing_count = 0;
    buf_size = tsize;

  }

public:

  //
  // no of entries available to write
  //
  // helper function for write_canreserve
  //
  size_t write_reservesize(){

    // insert your code here
    return buf_size - (w_writing_count - w_deleted_count + buf_size) % buf_size;

    // return 0;
  }

  //
  // Can write 'n' entries?
  //
  bool write_canreserve(size_t n){

    // insert your code here
    return write_reservesize() >= n;
    // return false;
  }

  //
  // Reserve 'n' entries for write
  //
  size_t write_reserve(size_t n){
    size_t w_cur_pos = w_writing_count;
    w_writing_count = (w_writing_count + n) % buf_size;
    return w_cur_pos % buf_size;
    // insert your code here

    // return 0;
  }

  //
  // Commit
  //
  // Read/Write shared memory data structure
  //
  void write_release(channel_t& ch){
    ch.shared_write_count = w_writing_count;
    // insert your code here

  }




public:

  //
  //
  // Read/Write shared memory data structure
  //
  void read_acquire(channel_t& ch){

    //insert your code here
    w_cached_read_count = ch.shared_read_count;

  }




  //
  // No of entires available to delete
  //
  size_t delete_reservesize(){
    //insert your code here
    return (w_cached_read_count - w_deleted_count + buf_size) % buf_size;
    // return 0;
  }

  //
  // Can delete 'n' entires?
  //
  bool delete_canreserve(size_t n){
    //insert your code here
    return delete_reservesize() >= n;
    // return false;
  }

  //
  // Reserve 'n' entires for deletion
  //
  size_t delete_reserve(size_t n){
    //insert your code here
    // TODO
    // return 0;
    int d_pos = w_deleting_count;
    w_deleting_count = (w_deleting_count + n) % buf_size;
    return d_pos;
  }


  //
  // Update the state, if any.
  //
  void delete_release(){
    //insert your code here
    w_deleted_count = w_deleting_count;


  }


};


//
// Consumer's (not shared)
//
//
struct readport_t{
public:

  //insert your code here
  int r_reading_count;
  int r_cached_write_count;
  int buf_size;

public:
  //
  // Initialize
  //
  readport_t(size_t tsize)
  {

    //insert your code here
    r_reading_count = 0;
    r_cached_write_count = 0;
    buf_size = tsize;
  }
  public:

  //
  // Read/Write shared memory data structure
  //
  void write_acquire(channel_t& ch){

    //insert your code here
    r_cached_write_count = ch.shared_write_count;
    

  }

  //
  // no of entries available to read
  //
  size_t read_reservesize(){

    //insert your code here
    return (r_cached_write_count - r_reading_count + buf_size) % buf_size;

    // return 0;
  }

  //
  // Can Read 'n' entires?
  //
  bool read_canreserve(size_t n){

    //insert your code here
    return read_reservesize() >= n;

    // return false;
  }

  //
  // Reserve 'n' entires to be read
  //
  size_t read_reserve(size_t n){

    //insert your code here
    size_t r_cur_pos = r_reading_count;
    r_reading_count = (r_reading_count + n) % buf_size;
    return (r_cur_pos % buf_size);
    // return 0;
  }

  //
  // Read/write shared memory data structure
  //
  void read_release(channel_t& ch){

    //insert your code here
    ch.shared_read_count = r_reading_count;
    

  }

};


