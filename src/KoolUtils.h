#include <stddef.h>

namespace koolutils
{
  typedef enum
  {
    API_METHOD_GET = 0b00000001,
    API_METHOD_POST = 0b00000010,
    API_METHOD_DELETE = 0b00000100,
    API_METHOD_PUT = 0b00001000,
    API_METHOD_PATCH = 0b00010000,
    API_METHOD_OPTIONS = 0b01000000,
    API_METHOD_ANY = 0b01111111,

    API_METHOD_UNKNOWN = -1
  } api_method_t;

#include <stddef.h>

  template <typename T>
  class LinkedListNode
  {
    T _value;

  public:
    LinkedListNode<T> *next;
    LinkedListNode(const T val) : _value(val), next(nullptr) {}
    ~LinkedListNode() {}
    const T &value() const { return _value; };
    T &value() { return _value; }
  };

  template <typename T, template <typename> class Item = LinkedListNode>
  class LinkedList
  {
  public:
    typedef Item<T> ItemType;
    typedef std::function<void(const T &)> OnRemove;
    typedef std::function<bool(const T &)> Predicate;

  private:
    ItemType *_root;
    OnRemove _onRemove;

    class Iterator
    {
      ItemType *_node;

    public:
      Iterator(ItemType *current = nullptr) : _node(current) {}
      Iterator(const Iterator &i) : _node(i._node) {}
      Iterator &operator++()
      {
        _node = _node->next;
        return *this;
      }
      bool operator!=(const Iterator &i) const { return _node != i._node; }
      const T &operator*() const { return _node->value(); }
      const T *operator->() const { return &_node->value(); }
    };

  public:
    typedef const Iterator ConstIterator;
    ConstIterator begin() const { return ConstIterator(_root); }
    ConstIterator end() const { return ConstIterator(nullptr); }

    LinkedList(OnRemove onRemove) : _root(nullptr), _onRemove(onRemove) {}
    ~LinkedList() {}
    void add(const T &t)
    {
      auto it = new ItemType(t);
      if (!_root)
      {
        _root = it;
      }
      else
      {
        auto i = _root;
        while (i->next)
          i = i->next;
        i->next = it;
      }
    }
    T &front() const
    {
      return _root->value();
    }

    bool isEmpty() const
    {
      return _root == nullptr;
    }
    size_t length() const
    {
      size_t i = 0;
      auto it = _root;
      while (it)
      {
        i++;
        it = it->next;
      }
      return i;
    }
    size_t count_if(Predicate predicate) const
    {
      size_t i = 0;
      auto it = _root;
      while (it)
      {
        if (!predicate)
        {
          i++;
        }
        else if (predicate(it->value()))
        {
          i++;
        }
        it = it->next;
      }
      return i;
    }
    const T *nth(size_t N) const
    {
      size_t i = 0;
      auto it = _root;
      while (it)
      {
        if (i++ == N)
          return &(it->value());
        it = it->next;
      }
      return nullptr;
    }
    bool remove(const T &t)
    {
      auto it = _root;
      auto pit = _root;
      while (it)
      {
        if (it->value() == t)
        {
          if (it == _root)
          {
            _root = _root->next;
          }
          else
          {
            pit->next = it->next;
          }

          if (_onRemove)
          {
            _onRemove(it->value());
          }

          delete it;
          return true;
        }
        pit = it;
        it = it->next;
      }
      return false;
    }
    bool remove_first(Predicate predicate)
    {
      auto it = _root;
      auto pit = _root;
      while (it)
      {
        if (predicate(it->value()))
        {
          if (it == _root)
          {
            _root = _root->next;
          }
          else
          {
            pit->next = it->next;
          }
          if (_onRemove)
          {
            _onRemove(it->value());
          }
          delete it;
          return true;
        }
        pit = it;
        it = it->next;
      }
      return false;
    }

    void free()
    {
      while (_root != nullptr)
      {
        auto it = _root;
        _root = _root->next;
        if (_onRemove)
        {
          _onRemove(it->value());
        }
        delete it;
      }
      _root = nullptr;
    }
  };

};
