//
// Created by comoon on 5/16/17.
//

#ifndef ZEUS_FUTURE_H
#define ZEUS_FUTURE_H

#include "Apply.h"
#include "Engine.h"
#include <cassert>


template <class T>
class State;

template <typename Func, typename T>
struct continuation final : task {
    continuation(Func&& func, State<T>&& state) : _state(std::move(state)), _func(std::move(func)) {}
    continuation(Func&& func) : _func(std::move(func)) {}
    virtual void run() noexcept override {
        _func(std::move(_state));
    }
    State<T> _state;
    Func _func;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static constexpr unsigned max_inlined_continuations = 256;
static unsigned future_avail_count = 256;

struct ready_future_marker {};
struct exception_future_marker {};

template <class T>
class Promise;

template <class T>
class Future;

template <typename T>
class State {
public:
    enum class state {
        invalid,
        future,
        result,
        exception,
    } _st = state::future;

    union any{
        any(){}
        ~any(){}
        std::tuple<T> value;
        std::exception_ptr ex;
    } _u;

    State() noexcept {}
    State(State&& x) noexcept : _st(x._st) {
        switch (_st) {
            case state::future:
                break;
            case state::result:
                new (&_u.value) std::tuple<T>(std::move(x._u.value));
                x._u.value.~tuple();
                break;
            case state::exception:
                new (&_u.ex) std::exception_ptr(std::move(x._u.ex));
                x._u.ex.~exception_ptr();
                break;
            case state::invalid:
                break;
            default:
                abort();
        }
        x._st = state::invalid;
    }
    ~State() noexcept {
        switch (_st) {
            case state::invalid:
                break;
            case state::future:
                break;
            case state::result:
                _u.value.~tuple();
                break;
            case state::exception:
                _u.ex.~exception_ptr();
                break;
            default:
                abort();
        }
    }

    State &operator = (State&& x) noexcept {
        if (this != &x) {
            this->~State();
            new (this) State(std::move(x));
        }
        return *this;
    }

    bool available() const noexcept { return _st == state::result || _st == state::exception; }
    bool failed() const noexcept { return _st == state::exception; }

    template <typename... A>
    void set(A&& ...a) {
        assert(_st == state::future);
        new (&_u.value) std::tuple<T>(std::forward<A>(a)...);
        _st = state::result;
    }

    void set_exception(std::exception_ptr ex) noexcept {
        assert(_st == state::future);
        new (&_u.ex) std::exception_ptr(ex);
        _st = state::exception;
    }

    std::tuple<T> get() && {
        assert(_st != state::future);
        if (_st == state::exception) {
            _st = state::invalid;
            auto ex = std::move(_u.ex);
            _u.ex.~exception_ptr();
            std::rethrow_exception(std::move(ex));
        }
        return std::move(_u.value);
    }

    std::exception_ptr get_exception() && noexcept {
        assert(_st == state::exception);
        _st = state::invalid;
        auto ex = std::move(_u.ex);
        _u.ex.~exception_ptr();
        return ex;
    }

    std::tuple<T> get_value() && noexcept {
        assert(_st == state::result);
        return std::move(_u.value);
    }

    void forward_to(Promise<T>& pr) noexcept {
        assert(_st != state::future);
        if (_st == state::exception) {
            pr.set_exception(std::move(_u.ex));
            _u.ex.~exception_ptr();
        } else {
            pr.set_value(std::move(_u.value));
            _u.value.~tuple();
        }
        _st = state::invalid;
    }
};

template <typename T, typename ... A>
Future<T> make_ready_future(A&& ... value) {
    return Future<T>(ready_future_marker(), std::forward<A>(value)...);
};

template <typename T>
Future<T> make_exception_future(std::exception_ptr ex) noexcept{
    return Future<T>(exception_future_marker(), std::move(ex));
}

template <typename T>
class futurize {
public:
    using type = Future<T>;
    using promise_type = Promise<T>;
    using value_type = std::tuple<T>;
    static inline type convert(T&& value) { return make_ready_future<T>(std::move(value)); }

    template<typename Func, typename... FuncArgs>
    static inline type apply(Func&& func, std::tuple<FuncArgs...>&& args) noexcept {
        try {
            return convert(::apply(std::forward<Func>(func), std::move(args)));
        } catch (...) {
            return make_exception_future(std::current_exception());
        }
    };

    template <typename Func, typename ... Args>
    static inline type apply(Func && func, Args&& ... args) noexcept {
        try{
            return convert(func(std::forward<Args>(args)...));
        }catch(...){
            return make_exception_future(std::current_exception());
        }
    };

    template <typename Arg>
    static type make_exception_future(Arg&& arg){
        return ::make_exception_future<T>(std::forward<Arg>(arg));
    }
};

template <typename T>
using futurize_t = typename futurize<T>::type;

template <typename T>
class Future {

private:
    Future(Promise<T>* pr) noexcept : _promise(pr) {
        _promise->_future = this;
    }

    template <typename ... A>
    Future(ready_future_marker, A&& ...a) : _promise(nullptr){
        _local_state.set(std::forward<A>(a)...);
    }

    Future(exception_future_marker, std::exception_ptr ex) :_promise(nullptr) {
        _local_state.set_exception(std::move(ex));
    }

public:

    Future(Future && x) noexcept : _promise(x._promise) {
        if (!_promise)
            _local_state = std::move(x._local_state);

        x._promise = nullptr;
        if (_promise) {
            _promise->_future = this;
        }
    }

    bool available() noexcept { return state()->available(); }
    bool failed() noexcept { return state()->failed(); }

    template<typename Func, typename Result = futurize_t<std::result_of_t<Func(T&&)>>>
    Result then(Func&& func) noexcept {
        using futurator = futurize<std::result_of_t<Func(T&&)>>;
        if (available() && (++future_avail_count % max_inlined_continuations)) {
            if (failed()) {
                return futurator::make_exception_future(get_available_state().get_exception());
            } else {
                return futurator::apply(std::forward<Func>(func), get_available_state().get_value());
            }
        }
        typename futurator::promise_type pr;
        auto fut = pr.get_future();
        try {
            schedule([pr = std::move(pr), func = std::forward<Func>(func)] (auto&& state) mutable {
                if (state.failed()) {
                    pr.set_exception(std::move(state).get_exception());
                } else {
                    futurator::apply(std::forward<Func>(func), std::move(state).get_value()).forward_to(std::move(pr));
                }
            });
        } catch (...) {
            abort();
        }
        return fut;
    };

private:
    State<T> get_available_state() noexcept {
        auto st = state();
        if (_promise) {
            _promise->_future = nullptr;
            _promise = nullptr;
        }
        return std::move(*st);
    }

    template <typename Func>
    void schedule(Func&& func) {
        if (state()->available()) {
            ::schedule(std::make_unique<continuation<Func, T>>(std::move(func), std::move(*state())));
        } else {
            assert(_promise);
            _promise->schedule(std::move(func));
            _promise->_future = nullptr;
            _promise = nullptr;
        }
    }

    void forward_to(Promise<T> &&pr) noexcept {
        if (state()->available()) {
            state()->forward_to(pr);
        } else {
            _promise->_future = nullptr;
            *_promise = std::move(pr);
            _promise = nullptr;
        }
    }

    State<T> * state() noexcept { return _promise ? _promise->_state : &_local_state; }

private:
    Promise<T> * _promise;
    State<T> _local_state;

    template <typename U>
    friend class Promise;

    template <typename U, typename... A>
    friend Future<U> make_ready_future(A&&... value);

    template <typename U>
    friend Future<U> make_exception_future(std::exception_ptr ex) noexcept;
};

template <typename T>
class Promise {

public:
    Future<T> get_future() noexcept {
        assert(!_future);
        return Future<T>(this);
    }

    template <typename ...A>
    void set_value(A&& ...a) noexcept {
        assert(_state);
        _state->set(std::forward<A>(a)...);
        make_ready();
    }

    void set_exception(std::exception_ptr ex) noexcept {
        assert(_state);
        _state->set_exception(std::move(ex));
        make_ready();
    }

private:
    void make_ready() noexcept {
        if (_task) {
            _state = nullptr;
            ::schedule(std::move(_task));
        }
    }

    template <typename Func>
    void schedule(Func&& func) {
        auto tws = std::make_unique<continuation<Func, T>>(std::move(func));
        _state = &tws->_state;
        _task = std::move(tws);
    }

private:
    Future<T> * _future = nullptr;
    State<T> _local_state;
    State<T> * _state;
    std::unique_ptr<task> _task;

    friend class Future<T>;
};

void schedule(std::unique_ptr<task> t) {
    engine().add_task(std::move(t));
}



#endif //ZEUS_FUTURE_H
