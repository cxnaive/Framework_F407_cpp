#ifndef _PUB_SUB_H_
#define _PUB_SUB_H_

#include <list>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

template <typename T>
class publisher;

template <typename T>
class subscriber;

template <typename T>
class message_pipe {
   public:
    using sptr = std::shared_ptr<message_pipe<T>>;
    using wptr = std::weak_ptr<message_pipe<T>>;
    friend class publisher<T>;
    friend class subscriber<T>;

    static sptr create(const std::string &name) {
        if (_map.find(name) != _map.end()) return nullptr;
        sptr m_obj = std::make_shared<message_pipe<T>>();
        _map.emplace(name, m_obj);
        return m_obj;
    }
    static sptr find(const std::string &name) {
        auto iter = _map.find(name);
        if (iter == _map.end())
            return nullptr;
        else
            return iter->second.lock();
    }
    static sptr find_or_create(const std::string &name) {
        auto iter = _map.find(name);
        if (iter != _map.end()) return iter->second.lock();
        sptr m_obj = std::make_shared<message_pipe<T>>();
        _map.emplace(name, m_obj);
        return m_obj;
    }

   private:
    std::list<publisher<T> *> pubs;
    std::list<subscriber<T> *> subs;
    static std::unordered_map<std::string, wptr> _map;
};

template <typename T>
__attribute__((visibility("default"))) inline std::unordered_map<std::string, typename message_pipe<T>::wptr> message_pipe<T>::_map;

template <typename T>
class publisher {
   private:
    using MsgManager = message_pipe<T>;
    typename MsgManager::sptr p_msg;

   public:
    publisher() = default;
    /**
     * @brief 发布器的构造函数
     * @param msg_name 消息名称
     */
    explicit publisher(const std::string &msg_name) {
        bind(msg_name);
    }

    /// 拷贝构造函数
    publisher(const publisher &other) : p_msg(other.p_msg) {
        p_msg->pubs.emplace_front(this);
    }

    /// 移动构造函数
    publisher(publisher &&other) noexcept : p_msg(other.p_msg) {
        other.reset();
        p_msg->pubs.emplace_front(this);
    }

    /// 析构函数
    ~publisher() { reset(); }

    /// 判断当前发布器是否绑定到某个消息
    explicit operator bool() {
        return p_msg != nullptr;
    }
    /// 重置发布器
    void reset() {
        if (!p_msg) return;
        p_msg->pubs.remove(this);
        p_msg.reset();
    }
    /**
     * @brief 绑定当前发布器到某个名称的消息
     * @param msg_name 消息名称
     */
    void bind(const std::string &msg_name) {
        reset();
        p_msg = MsgManager::find_or_create(msg_name);
        p_msg->pubs.emplace_front(this);
    }

    /**
     * @brief 发布一条消息
     * @param obj 待发布的消息消息
     */
    void push(const T &obj) {
        if (!p_msg) return;
        for (auto &sub : p_msg->subs) {
            sub->write_obj(obj);
        }
    }
};

template <typename T>
class subscriber {
    friend publisher<T>;

   private:
    using MsgManager = message_pipe<T>;
    size_t fifo_size;
    T temp_value;
    std::queue<T> fifo;
    typename MsgManager::sptr p_msg;

    void write_obj(const T &obj) {
        if (fifo_size > 0 && fifo.size() >= fifo_size) {
            fifo.pop();
        }
        fifo.push(obj);
    }

   public:
    subscriber() = default;
    /**
     * @details 构造函数
     * @param msg_name 消息名称
     * @param max_fifo_size 最大消息长度
     */
    explicit subscriber(const std::string &msg_name, size_t size = 1) : fifo_size(size) {
        bind(msg_name);
    }

    /// 拷贝构造函数
    subscriber(const subscriber &other) : fifo_size(other.fifo_size), fifo(other.fifo), p_msg(other.p_msg) {
        p_msg->subs.emplace_front(this);
    }

    /// 移动构造函数
    subscriber(subscriber &&other) noexcept : fifo_size(other.fifo_size), fifo(std::move(other.fifo)), p_msg(other.p_msg) {
        other.reset();
        p_msg->subs.emplace_front(this);
    }

    /// 析构函数
    ~subscriber() { reset(); }

    /// 判断当前订阅器是否绑定到某个消息
    explicit operator bool() {
        return p_msg != nullptr;
    }

    /// 重置订阅器
    void reset() {
        if (!fifo.empty()) fifo = std::queue<T>();
        if (!p_msg) return;
        p_msg->subs.remove(this);
        p_msg.reset();
    }

    /**
     * @brief 绑定当前订阅器到某个名称的消息
     * @param msg_name 消息名称
     */
    void bind(const std::string &msg_name) {
        reset();
        p_msg = MsgManager::find_or_create(msg_name);
        p_msg->subs.emplace_front(this);
    }

    /**
     * @brief 设置队列长度，size==0则不限制最大长度
     * @param size 最大队列长度
     */
    void set_fifo_size(size_t size) {
        fifo_size = size;
    }
    /**
     * @brief 读取当前最大队列长度
     * @return 当前最大队列长度
     */
    size_t get_fifo_size() {
        return fifo_size;
    }

    /**
     * @brief 尝试获取一条消息
     * @details 如果没有获得消息，返回NULL
     * @return 读取到的消息
     */
    T *pop() {
        if (!p_msg) return NULL;
        if (fifo.empty()) return NULL;
        temp_value = std::move(fifo.front());
        fifo.pop();
        return &temp_value;
    }
    /**
     * @brief 清空接收缓冲区
     */
    void clear() {
        fifo = std::queue<T>();
    }
};

#endif