#ifndef LIST_HPP
#define LIST_HPP

template<int SIZE>
class List {
private:
  int _tab_x[SIZE];
  int _tab_y[SIZE];
  int _begin;
  int _size;
public:
  List<SIZE>(void) {
    _begin = 0;
    _size = 0;
  }

  void addHead(int x, int y) {
    if (_size > SIZE -1)
      return;
    _tab_x[(_begin + _size) % SIZE] = x;
    _tab_y[(_begin + _size) % SIZE] = y;
    _size++;
  }

  void removeTail(int *x, int *y) {
    *x = _tab_x[_begin];
    *y = _tab_y[_begin];
    _size --;
    _begin++;
    _begin%=SIZE;
  }

  bool isEmpty(void) {
    return  _size == 0;
  }

  void clean(void) {
    _begin = 0;
    _size = 0;    
  }
};

#endif //LIST_HPP
