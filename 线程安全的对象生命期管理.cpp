编写线程安全的类
{
    方式：使用同步原语保护内部状态
    线程安全的定义：Thread safe meant it will work correctly if several threads will try to access it together

    举一个线程不安全的类的例子
    {
        class NotSafe
        {
        private:
            int* x;
        public:
            NotSafe(){x = new int;}
            void reallocate()
            {
                delete x;
                x = new x;
            }
            int access(){return *x;}
        }
        // If thread calls reallocate and after line 16 will be executed (deleting x) 
        // another thread will request access() function which will try to dereference invalid pointer. Which can lead to crash.
    }

    Not Thread safe singleton implementation in C++
    {
        #include<iostream>
        using namespace std;

        template<typename T>
        class Singleton
        {
        private:
            Singleton(const Singleton&);
            Singleton& operator=(const Singleton&);
        protected:
            Singleton(){}
            virtual ~Singleton(){}
        public:
            static T& instance()
            {
                static T theInstance;
                return theInstance;
            }
        };

        class MyClass: public Singleton<MyClass>
        {
        private:
            int x;
        protected:
            friend class Singleton<MyClass>;
            MyClass(){x = 0;}
        public:
            void setValue(int n){x = n;}
            int getValue() const{return x;}
        }
    }

    hello world
    {
        1.单线程
        {
            #include<iostream>
            int main()
            {
                std::out << "Hello world" << endl;
            }
        }

        2.启动单线程显示hello world
        {
            #include<iostream>
            #include<thread>
            void hello()
            {
                std::out << "Hello Concurrent World" << endl;
            }
            int main()
            {
                std::thread t(hello);
                t.join();
            }
        }
    }
}

线程管理
{
    1.启动新线程
    {
        class backgroud_task
        {
        public:
            void operator()() const
            {
                do_something();
                do_something_else();
            }
        };

        backgroud_task f;
        thread my_thread(f);
    }

    2.等待线程完成
    {
        方式：使用RAII进行管理
        {
            class thread_guard
            {
            private:
                thread& t;

                thread_guard(const thread_guard&);
                thread_guard& operator=(const thread_guard&);
            
            public:
                explicit thread_guard(thread& t_):
                t(t_)
                {}
                ~thread_guard()
                {
                    if(t.joinable())
                        t.join();
                }
            };

            struct func
            {
                int& i;
                fun(int& i_):i(i_){}
                void operator()()
                {
                    for(size_t j = 0; j < 1000000; ++j)
                    do_something(j);
                }
            };

            void f()
            {
                int some_local_state = 0;
                func my_func(some_local_state);
                thread t(my_func);
                thread_guard g(t);
                do_something_in_current_thread();
            }
        }
    }

    3.在后台运行线程
    {
        例子，使用分离线程去处理其它文档
        {
            void edit_document(const string& filename)
            {
                open_document_and_display_gui(filename);
                while(!done_editing())
                {
                    user_command cmd = get_user_input();
                    if(cmd.type == open_new_document)
                    {
                        const string new_name = get_filename_from_user();
                        thread t(edit_document, new_name);
                        t.detach(); //分离线程
                    }
                    else
                        process_user_input(cmd);
                }
            }
        }
    }

    4.向线程函数传递参数
    {
        1.最简单的传递参数的例子
        {
            void f(int i, const string& s);
            thread t(f, 3, "hello world");
        }

        2.上个例子中参数的字面值转化为string对象可能导致某些未定义行为，需要进行显式转化
        {
            void f(int i, const string& s);
            thread t(f, 3, string("hello world"));
        }

        3.当参数传递引用时，thread的构造函数并不会知道，所以需要使用std::ref()这个函数
        {
            void update_data_for_widget(widget_id w, widget_data& data);
            void oops_again(widget_id w)
            {
                widget_data data;
                thread t(update_data_for_widget, w, std::ref(data));
                displat_status();
                t.join();
                process_widget_data(data);
            }
        } 
    }

    5.转移线程所有权
    {
        一个简单的例子，当线程是一个有命名的线程对象时，需要使用std::move()
        {
            void f(std::thread t);
            void some_function();
            thread t1(some_function);
            thread t2 = std::move(t1);
            f(thread(some_function));
        }

        利用线程支持移动的特性，可以配合RAII来确保线程程序在退出前完成
        {
            class scoped_thread
            {
            private:
                thread t;
                
                scoped_thread(const scoped_thread&);
                scoped_thread& operator=(const scoped_thread&);

            public:
                explicit scoped_thread(thread t_):
                t(std::move(t_))
                {
                    if(!t.joinable())
                        throw std::logic_error("No thread!");
                }
                ~scoped_thread()
                {
                    t.join();
                }
            };
            struct func;
            void f()
            {
                int some_local_state;
                scoped_thread t(std::thread(func(some_local_state)));
                do_something_in_current_thread();
            }
        }
    }
}