#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

#define Size_of_block 1800
#define Min_size 850
namespace sjtu
{
    int memcheck=0;
    template<class T>
    class deque
    {
    private:
        class node;
        class _pair
        {
        public:
            _pair(node *node, int pos) : _pos(pos), _node(node)
            {}

            node *_node;
            int _pos;
        };

        class node
        {
        private:
            void merge_next()
            {
                for(int i=0; i<next->amount; i++)
                {
                    storage[amount]=next->storage[i];
                    next->storage[i]=nullptr;
                    amount++;
                    //next->amount--;
                }
                next->amount=0;
                node *tem=next->next;
                delete next;
                next=tem;
                next->front=this;
            }

            void merge_front()
            {
                for(int i=amount-1; i>=0; i--)
                {
                    storage[i+front->amount]=storage[i];
                }
                for(int i=0; i<front->amount; i++)
                {
                    storage[i]=front->storage[i];
                    front->storage[i]=nullptr;
                    amount++;
                    //front->amount--;
                }
                front->amount=0;
                node *tem=front->front;
                delete front;
                front=tem;
                tem->next=this;
            }

            void borrow_next()
            {
                storage[amount]=next->storage[0];
                next->storage[0]=nullptr;
                amount++;
                next->amount--;
                for(int i=0; i<next->amount; i++)
                {
                    next->storage[i]=next->storage[i+1];
                }
            }

            void borrow_front()
            {
                for(int i=amount; i>0; i--)
                {
                    storage[i]=storage[i-1];
                }
                amount++;
                storage[0]=front->storage[front->amount-1];
                front->storage[front->amount-1]=nullptr;
                front->amount--;
            }

        public:
            T *storage[Size_of_block];
            int amount=0;
            node *front=nullptr;
            node *next=nullptr;
            bool end;

            node(bool x) : end(true)
            {
                amount=0;
                front=nullptr;
                next=nullptr;
                for(int i=0; i<Size_of_block; i++)
                    storage[i]=nullptr;
            }

            node() : end(false)
            {
                amount=0;
                front=nullptr;
                next=nullptr;
                for(int i=0; i<Size_of_block; i++)
                    storage[i]=nullptr;
            }

            ~node()
            {
                for(int i=0; i<amount; i++)
                {
                    memcheck--;
                    delete storage[i];
                }
            }

            void operator=(const node &other)
            {
                if(this==&other)return;
                end=other.end;
                next=other.next;
                front=other.front;
                amount=other.amount;
                for(int i=0; i<amount; i++)
                {
                    memcheck++;
                    storage[i]=new T(*(other.storage[i]));
                }
                for(int i=amount; i<Size_of_block; i++)
                {
                    storage[i]=nullptr;
                }
            }

            void operator=(node &&other)
            {
                if(this==&other)return;
                end=other.end;
                next=other.next;
                front=other.front;
                amount=other.amount;
                for(int i=0; i<amount; i++)
                {
                    storage[i]=other.storage[i];
                    other.storage[i]=nullptr;
                }
                for(int i=amount; i<Size_of_block; i++)
                {
                    storage[i]=nullptr;
                }
            }

            bool is_head()
            {
                if(end&&front==nullptr)return true;
                else return false;
            }

            bool is_tail()
            {
                if(end&&next==nullptr)return true;
                else return false;
            }

            _pair remove(int number)
            {
                memcheck--;
                delete storage[number];
                for(int i=number; i<amount-1; i++)
                {
                    storage[i]=storage[i+1];
                }
                storage[amount-1]=nullptr;
                amount--;
                if(amount<Min_size)
                {
                    if(!next->end)
                    {
                        if(next->amount+amount<=Size_of_block)merge_next();
                        //else borrow_next();
                        if(number==amount)return _pair(next, 0);
                        return _pair(this, number);
                    } else if(!front->end)
                    {
                        if(front->amount+amount<=Size_of_block)
                        {
                            int _number=front->amount;
                            merge_front();
                            if(number+_number==amount)return _pair(next, 0);
                            return _pair(this, number+_number);
                        } else
                        {
                            //borrow_front();
                            if(number==amount)return _pair(next, 0);
                            return _pair(this, number);
                        }
                    }
                }
                if(number==amount)return _pair(next, 0);
                return _pair(this, number);
            }

            _pair insert(int number, const T &value)
            {
                if(amount==Size_of_block)
                {
                    node *tem=new node;
                    for(int i=Size_of_block/2; i<Size_of_block; i++)
                    {
                        tem->storage[tem->amount]=storage[i];
                        storage[i]=nullptr;
                        amount--;
                        tem->amount++;
                    }
                    tem->next=next;
                    tem->front=this;
                    next=tem;
                    tem->next->front=tem;
                    if(number>=Size_of_block/2)
                    {
                        int _n=number-Size_of_block/2;
                        return tem->insert(_n, value);
                    } else
                    {
                        return insert(number, value);
                    }
                } else
                {
                    for(int i=amount; i>number; i--)
                    {
                        storage[i]=storage[i-1];
                    }
                    amount++;
                    memcheck++;
                    storage[number]=new T(value);
                    return _pair(this, number);
                }
            }

            T &at(int number) const
            {
                if(storage[number]==nullptr)throw invalid_iterator();
                return *(storage[number]);
            }

            void cpy_node(const node *o)
            {
                if(this==o)return;
                amount=o->amount;
                end=o->end;
                for(int i=0; i<o->amount; i++)
                {
                    memcheck++;
                    storage[i]=new T(*(o->storage[i]));
                }
                for(int i=amount; i<Size_of_block; i++)
                {
                    storage[i]=nullptr;
                }
            }

            T &give_beg()
            {
                return *(storage[0]);
            }

            T &give_end()
            {
                return *(storage[amount-1]);
            }
        };

        node *head;
        node *tail;
        int cnt=0;

        void rebuild_space()
        {
            auto it=head;
            while(it!=tail)
            {
                auto tem=it->next;
                delete it;
                it=tem;
            }
            delete it;
            head=new node(true);
            tail=new node(true);
            head->next=new node;
            tail->front=head->next;
            head->next->front=head;
            head->next->next=tail;
            cnt=0;
        }

        void cpy_deque(const deque &other)
        {
            node *it=other.head;
            node *write=head;
            delete write->next;
            while(it->next!=other.tail)
            {
                write->next=new node;
                write->next->front=write;
                write=write->next;
                it=it->next;
                write->cpy_node(it);
            }
            write->next=tail;
            tail->front=write;
            cnt=other.cnt;
        }

    public:
        class const_iterator;

        class iterator
        {
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            int len;
            int pos;
            deque *father;
            node *block;
            friend deque;
        public:
            /**
             * return a new iterator which pointer n-next elements
             *   if there are not enough elements, iterator becomes invalid
             * as well as operator-
             */
            iterator()
            {
                len=0;
                pos=0;
                father=nullptr;
                block=nullptr;
            }

            iterator(deque *_father, node *_block, int _pos, int _len=-1) : father(_father), block(_block), pos(_pos),
                                                                            len(_len)
            {
                if(len==-1)
                {
                    len=0;
                    node *it=father->head;
                    while(it!=block)
                    {
                        len+=it->amount;
                        it=it->next;
                    }
                    len+=pos+1;
                }
            }

            iterator(const iterator &other) : father(other.father), block(other.block), pos(other.pos), len(other.len)
            {}

            iterator operator+(const int &n) const
            {
                if(n==0)return iterator(*this);
                if(n<0)return (*this)-(-n);
                int target=pos+n;
                node *tem=block;
                while(target>=tem->amount)
                {
                    target-=tem->amount;
                    tem=tem->next;
                    if(tem->end)
                    {
                        if(target!=0)throw invalid_iterator();
                        else
                        {
                            return iterator(father, tem->front, tem->front->amount, len+n);
                        }
                    }
                }
                return iterator(father, tem, target, len+n);
            }

            iterator operator-(const int &n) const
            {
                if(n==0)return iterator(*this);
                if(n<0)return (*this)+(-n);
                int target=pos-n;
                node *tem=block;
                while(target<0)
                {
                    if(tem->front->end)throw invalid_iterator();
                    target+=tem->front->amount;
                    tem=tem->front;
                }
                return iterator(father, tem, target, len-n);
            }

            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invalid_iterator.
            int operator-(const iterator &rhs) const
            {
                if(father!=rhs.father)throw invalid_iterator();
                return len-rhs.len;
            }

            iterator &operator+=(const int &n)
            {
                if(n==0)return *this;
                if(n<0)
                {
                    (*this)-=(-n);
                    return *this;
                }
                int target=pos+n;
                while(target>=block->amount)
                {
                    if(target==block->amount&&block->next->end)
                    {
                        pos=block->amount;
                        len=len+n;
                        return *this;
                    }
                    target-=block->amount;
                    block=block->next;
                    if(block->end)throw invalid_iterator();
                }
                len=len+n;
                pos=target;
                return *this;
            }

            iterator &operator-=(const int &n)
            {
                if(n==0)return *this;
                if(n<0)
                {
                    (*this)+=(-n);
                    return *this;
                }
                int target=pos-n;
                while(target<0)
                {
                    if(block->front->end)throw invalid_iterator();
                    target+=block->front->amount;
                    block=block->front;
                }
                pos=target;
                len=len-n;
                return *this;
            }

            /**
             * iter++
             */
            iterator operator++(int)
            {
                iterator ans(*this);
                (*this)+=1;
                return ans;
            }

            /**
             * ++iter
             */
            iterator &operator++()
            {
                (*this)+=1;
                return *this;
            }

            /**
             * iter--
             */
            iterator operator--(int)
            {
                iterator ans(*this);
                (*this)-=1;
                return ans;
            }

            /**
             * --iter
             */
            iterator &operator--()
            {
                (*this)-=1;
                return *this;
            }

            /**
             * *it
             * 		throw if iterator is invalid
             */
            T &operator*() const
            {
                if(father==nullptr||block->end)throw invalid_iterator();
                return block->at(pos);
            }

            /**
             * it->field
             * 		throw if iterator is invalid
             */
            T *operator->() const noexcept
            {
                /*if(father==nullptr||block->end)throw invalid_iterator();
                return block->storage[pos];*/
                return &(operator*());
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const
            {
                if(father==rhs.father&&block==rhs.block&&pos==rhs.pos)return true;
                else return false;
            }

            bool operator==(const const_iterator &rhs) const
            {
                if(father==rhs.father&&block==rhs.block&&pos==rhs.pos)return true;
                else return false;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const
            {
                return !((*this)==rhs);
            }

            bool operator!=(const const_iterator &rhs) const
            {
                return !((*this)==rhs);
            }

            void debug()
            {
                printf("======%x %d======\n", block, pos);
            }
        };

        class const_iterator
        {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            // data members.
            int len;
            int pos;
            const deque *father;
            const node *block;
            friend deque;
        public:
            void debug()
            {
                printf("======%x %d======\n", block, pos);
            }

            const_iterator()
            {
                len=0;
                pos=0;
                father=nullptr;
                block=nullptr;
            }

            const_iterator(const const_iterator &other) : father(other.father), block(other.block), pos(other.pos),
                                                          len(other.len)
            {}

            const_iterator(const iterator &other) : father(other.father), block(other.block), pos(other.pos),
                                                    len(other.len)
            {}

            const_iterator(const deque *_father, const node *_block, int _pos, int _len=-1) : father(_father),
                                                                                              block(_block), pos(_pos),
                                                                                              len(_len)
            {
                if(len==-1)
                {
                    len=0;
                    node *it=father->head;
                    while(it!=block)
                    {
                        len+=it->amount;
                        it=it->next;
                    }
                    len+=pos+1;
                }
            }

            const_iterator operator+(const int &n) const
            {
                if(n==0)return const_iterator(*this);
                if(n<0)return (*this)-(-n);
                int target=pos+n;
                //todo change
                const node *tem=block;
                while(target>=tem->amount)
                {
                    target-=tem->amount;
                    tem=tem->next;
                    if(tem->end)
                    {
                        if(target!=0)throw invalid_iterator();
                        else
                        {
                            return const_iterator(father, tem->front, tem->front->amount, len+n);
                        }
                    }
                }
                return const_iterator(father, tem, target, len+n);
            }

            const_iterator operator-(const int &n) const
            {
                if(n==0)return const_iterator(*this);
                if(n<0)return (*this)+(-n);
                int target=pos-n;
                //todo change
                const node *tem=block;
                while(target<0)
                {
                    if(tem->front->end)throw invalid_iterator();
                    target+=tem->front->amount;
                    tem=tem->front;
                }
                return const_iterator(father, tem, target, len-n);
            }

            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invalid_iterator.
            int operator-(const const_iterator &rhs) const
            {
                if(father!=rhs.father)throw invalid_iterator();
                return len-rhs.len;
            }

            int operator-(const iterator &rhs) const
            {
                if(father!=rhs.father)throw invalid_iterator();
                return len-rhs.len;
            }

            const_iterator &operator+=(const int &n)
            {
                if(n==0)return *this;
                if(n<0)
                {
                    (*this)-=(-n);
                    return *this;
                }
                int target=pos+n;
                while(target>=block->amount)
                {
                    if(target==block->amount&&block->next->end)
                    {
                        pos=block->amount;
                        len=len+n;
                        return *this;
                    }
                    target-=block->amount;
                    block=block->next;
                    if(block->end)throw invalid_iterator();
                }
                len=len+n;
                pos=target;
                return *this;
            }

            const_iterator &operator-=(const int &n)
            {
                if(n==0)return *this;
                if(n<0)
                {
                    (*this)+=(-n);
                    return *this;
                }
                int target=pos-n;
                while(target<0)
                {
                    if(block->front->end)throw invalid_iterator();
                    target+=block->front->amount;
                    block=block->front;
                }
                pos=target;
                len=len-n;
                return *this;
            }

            /**
             * iter++
             */
            const_iterator operator++(int)
            {
                const_iterator ans(*this);
                (*this)+=1;
                return ans;
            }

            /**
             * ++iter
             */
            const_iterator &operator++()
            {
                (*this)+=1;
                return *this;
            }

            /**
             * iter--
             */
            const_iterator operator--(int)
            {
                const_iterator ans(*this);
                (*this)-=1;
                return ans;
            }

            /**
             * --iter
             */
            const_iterator &operator--()
            {
                (*this)-=1;
                return *this;
            }

            /**
             * *it
             * 		throw if iterator is invalid
             */
            const T &operator*() const
            {
                if(father==nullptr||block->end)throw invalid_iterator();
                return block->at(pos);
            }

            /**
             * it->field
             * 		throw if iterator is invalid
             */
            const T *operator->() const noexcept
            {
                /*if(father==nullptr||block->end)throw invalid_iterator();
                return block->storage[pos];*/
                return &(operator*());
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const
            {
                if(father==rhs.father&&block==rhs.block&&pos==rhs.pos)return true;
                else return false;
            }

            bool operator==(const const_iterator &rhs) const
            {
                if(father==rhs.father&&block==rhs.block&&pos==rhs.pos)return true;
                else return false;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const
            {
                return !((*this)==rhs);
            }

            bool operator!=(const const_iterator &rhs) const
            {
                return !((*this)==rhs);
            }
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.
        };

        /**
         * Constructors
         */
        deque()
        {
            head=new node(true);
            tail=new node(true);
            head->next=new node;
            tail->front=head->next;
            head->next->front=head;
            head->next->next=tail;
        }

        deque(const deque &other)
        {
            head=new node(true);
            tail=new node(true);
            cpy_deque(other);
        }

        /**
         * Deconstruct
         */
        ~deque()
        {
            auto it=head;
            while(it!=tail)
            {
                auto tem=it->next;
                delete it;
                it=tem;
            }
            delete it;
        }

        /**
         * TODO assignment operator
         */
        deque &operator=(const deque &other)
        {
            if(this==&other)return *this;
            rebuild_space();
            cpy_deque(other);
            return *this;
        }

        /**
         * access specified element with bounds checking
         * throw index_out_of_bound if out of bound.
         */
        T &at(const size_t &pos)
        {
            auto iter=head->next;
            int sum=0;
            while(!iter->end)
            {
                sum+=iter->amount;
                if(sum>pos)
                {
                    return *(iter->storage[pos-sum+iter->amount]);
                }
                iter=iter->next;
            }
            throw index_out_of_bound();
        }

        const T &at(const size_t &pos) const
        {
            auto iter=head->next;
            int sum=0;
            while(!iter->end)
            {
                sum+=iter->amount;
                if(sum>pos)
                {
                    return *(iter->storage[pos-sum+iter->amount]);
                }
                iter=iter->next;
            }
            throw index_out_of_bound();
        }

        T &operator[](const size_t &pos)
        {
            auto iter=head->next;
            int sum=0;
            while(!iter->end)
            {
                sum+=iter->amount;
                if(sum>pos)
                {
                    return *(iter->storage[pos-sum+iter->amount]);
                }
                iter=iter->next;
            }
            throw index_out_of_bound();
        }

        const T &operator[](const size_t &pos) const
        {
            auto iter=head->next;
            int sum=0;
            while(!iter->end)
            {
                sum+=iter->amount;
                if(sum>pos)
                {
                    return *(iter->storage[pos-sum+iter->amount]);
                }
                iter=iter->next;
            }
            throw index_out_of_bound();
        }

        /**
         * access the first element
         * throw container_is_empty when the container is empty.
         */
        const T &front() const
        {
            if(head->next->amount==0)throw container_is_empty();
            return head->next->give_beg();
        }

        /**
         * access the last element
         * throw container_is_empty when the container is empty.
         */
        const T &back() const
        {
            if(tail->front->amount==0)throw container_is_empty();
            return tail->front->give_end();
        }

        /**
         * returns an iterator to the beginning.
         */
        iterator begin()
        {
            return iterator(this, head->next, 0);
        }

        const_iterator cbegin() const
        {
            return const_iterator(this, head->next, 0);
        }

        /**
         * returns an iterator to the end.
         */
        iterator end()
        {
            return iterator(this, tail->front, tail->front->amount, cnt+1);
        }

        const_iterator cend() const
        {
            return const_iterator(this, tail->front, tail->front->amount, cnt+1);
        }

        /**
         * checks whether the container is empty.
         */
        bool empty() const
        {
            if(tail->front->amount==0)return true;
            else return false;
        }

        /**
         * returns the number of elements
         */
        size_t size() const
        {
            return cnt;
            int sum=0;
            node *it=head;
            while(it!=tail)
            {
                sum+=it->amount;
                it=it->next;
            }
            return sum;
        }

        /**
         * clears the contents
         */
        void clear()
        {
            rebuild_space();
            cnt=0;
        }

        /**
         * inserts elements at the specified local on in the container.
         * inserts value before pos
         * returns an iterator pointing to the inserted value
         *     throw if the iterator is invalid or it point to a wrong place.
         */
        iterator insert(iterator pos, const T &value)
        {
            if(pos.father!=this)throw invalid_iterator();
            _pair tem=pos.block->insert(pos.pos, value);
            cnt++;
            return iterator(this, tem._node, tem._pos);
        }

        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos)
        {
            if(pos.father!=this)throw invalid_iterator();
            if(pos.block->end)throw invalid_iterator();
            if(pos.pos>=pos.block->amount)throw invalid_iterator();
            _pair tem=pos.block->remove(pos.pos);
            cnt--;
            if(tem._node->next==nullptr)return iterator(this, tem._node->front, tem._node->front->amount);
            return iterator(this, tem._node, tem._pos);
        }

        /**
         * adds an element to the end
         */
        void push_back(const T &value)
        {
            _pair tem=tail->front->insert(tail->front->amount, value);
//            return iterator(this,tem._node,tem._pos);
            cnt++;
        }

        /**
         * removes the last element
         *     throw when the container is empty.
         */
        void pop_back()
        {
            if(empty())throw container_is_empty();
            /*static int cnt=0;
            cnt++;
            printf("%d %x\n",cnt,tail->front);
            printf("%d %d\n",cnt,tail->front->amount-1);*/
            _pair tem=tail->front->remove(tail->front->amount-1);
//            return iterator(this,tem._node,tem._pos);
            cnt--;
        }

        /**
         * inserts an element to the beginning.
         */
        void push_front(const T &value)
        {
            _pair tem=head->next->insert(0, value);
//            return iterator(this,tem._node,tem._pos);
            cnt++;
        }

        /**
         * removes the first element.
         *     throw when the container is empty.
         */
        void pop_front()
        {
            if(empty())throw container_is_empty();
            _pair tem=head->next->remove(0);
            cnt--;
//            return iterator(this,tem._node,tem._pos);
        }
    };

}

#endif