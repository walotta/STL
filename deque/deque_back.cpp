#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#define Size_of_block 10
#define Min_size 3
namespace sjtu {
    template<class T>
    class deque {
    private:
        class node
        {
        public:
            T* storage[Size_of_block];
            int amount=0;
            node* front=nullptr;
            node* next=nullptr;
        };
        node* head=nullptr;
        node* tail=nullptr;
        void delete_space()
        {
            auto iter=head;
            while(iter!=nullptr)
            {
                for(int i=0;i<iter->amount;i++)
                    delete iter->storage[i];
                iter=iter->next;
            }
            head=nullptr;
            tail=nullptr;
        }
        void copy_space(const deque &other)
        {
            node* iter=other.head;
            if(iter!=nullptr)
            {
                head=new node;
                auto copy=head;
                while(iter!=nullptr)
                {
                    copy->amount=iter->amount;
                    for(int i=0;i<copy->amount;i++)
                        copy->storage[i]=new T(*(iter->storage[i]));
                    iter=iter->next;
                    if(iter!=nullptr)
                    {
                        copy->next=new node;
                        copy->next->front=copy;
                        copy=copy->next;
                    }else
                    {
                        tail=copy;
                    }
                }
            }
        }
    public:
        class const_iterator;
        class iterator {
            friend deque;
        private:
            /**
             *   add data members
             *   just add whatever you want.
             */
            deque* father=nullptr;
            int pos=-1;
            node* block=nullptr;
        public:
            iterator():father(nullptr),pos(-1),block(nullptr){}
            iterator(deque* _father,int _pos,node* _block):father(_father),pos(_pos),block(_block){}
            iterator(const iterator& other):father(other.father),pos(other.pos),block(other.block){}
            /**
             * return a new iterator which pointer n-next elements
             *   if there are not enough elements, iterator becomes invalid
             * as well as operator-
             */
            iterator operator+(const int &n) const {
                int len=pos+n;
                node* tem=block;
                while(len>=tem->amount)
                {
                    len-=tem->amount;
                    tem=tem->next;
                    if(tem==nullptr)throw invalid_iterator();
                }
                return iterator(father,len,tem);
            }
            iterator operator-(const int &n) const {
                int len=pos-n;
                node* tem=block;
                while(len<0)
                {
                    if(tem->front==nullptr)throw invalid_iterator();
                    len+=tem->front->amount;
                    tem=tem->front;
                }
                return iterator(father,len,tem);
            }
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                if(father!=rhs.father)throw invalid_iterator();
                node* tem;
                int len=0;
                tem=rhs.block;
                while(tem!=block)
                {
                    if(tem==nullptr)
                    {
                        len=-1;
                        break;
                    }
                    len+=tem->amount;
                    tem=tem->next;
                }
                if(len==-1)
                {
                    tem=block;
                    while(tem!=rhs.block)
                    {
                        if(tem==nullptr)
                        {
                            throw invalid_iterator();
                        }
                        len+=tem->amount;
                        tem=tem->next;
                    }
                    len+=rhs.pos-pos;
                    return len;
                }else
                {
                    len+=pos-rhs.pos;
                    return len;
                }
            }
            iterator& operator+=(const int &n) {
                int len=pos+n;
                while(len>=block->amount)
                {
                    len-=block->amount;
                    block=block->next;
                    if(block==nullptr)throw invalid_iterator();
                }
                pos=len;
                return *this;
            }
            iterator& operator-=(const int &n) {
                int len=pos-n;
                while(len<0)
                {
                    if(block->front==nullptr)throw invalid_iterator();
                    len+=block->front->amount;
                    block=block->front;
                }
                pos=len;
                return *this;
            }
            /**
             *  iter++
             */
            iterator operator++(int) {
                iterator ans(*this);
                (*this)+=1;
                return ans;
            }
            /**
             * TODO ++iter
             */
            iterator& operator++() {
                (*this)+=1;
                return *this;
            }
            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator ans(*this);
                (*this)-=1;
                return ans;
            }
            /**
             * TODO --iter
             */
            iterator& operator--() {
                (*this)-=1;
                return *this;
            }
            /**
             * TODO *it
             * 		throw if iterator is invalid
             */
            T& operator*() const {
                if(block->storage[pos]==nullptr)throw invalid_iterator();
                return *(block->storage[pos]);
            }
            /**
             * TODO it->field
             * 		throw if iterator is invalid
             */
            T* operator->() const noexcept {
                return block->storage[pos];
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
                if(father==rhs.father&&block==rhs.block&&pos==rhs.pos)return true;
                else return false;
            }
            bool operator==(const const_iterator &rhs) const {
                if(father==rhs.father&&block==rhs.block&&pos==rhs.pos)return true;
                else return false;
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !((*this)==rhs);
            }
            bool operator!=(const const_iterator &rhs) const {
                return !((*this)==rhs);
            }
        };
        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
            friend deque;
        private:
            // data members.
            const deque* father=nullptr;
            int pos=-1;
            node* block=nullptr;
        public:
            const_iterator():father(nullptr),pos(-1),block(nullptr){}
            const_iterator(deque* _father,int _pos,node* _block):father(_father),pos(_pos),block(_block) {}
            const_iterator(const const_iterator &other):father(other.father),pos(other.pos),block(other.block) {}
            const_iterator(const iterator &other):father(other.father),pos(other.pos),block(other.block) {}
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.
            const_iterator operator+(const int &n) const {
                int len=pos+n;
                node* tem=block;
                while(len>=tem->amount)
                {
                    len-=tem->amount;
                    tem=tem->next;
                    if(tem==nullptr)throw invalid_iterator();
                }
                return iterator(father,len,tem);
            }
            const_iterator operator-(const int &n) const {
                int len=pos-n;
                node* tem;
                while(len<0)
                {
                    if(tem->front==nullptr)throw invalid_iterator();
                    len+=tem->front->amount;
                    tem=tem->front;
                }
                return iterator(father,len,tem);
            }
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const const_iterator &rhs) const {
                if(father!=rhs.father)throw invalid_iterator();
                node* tem;
                int len=0;
                tem=rhs.block;
                while(tem!=block)
                {
                    if(tem==nullptr)
                    {
                        len=-1;
                        break;
                    }
                    len+=tem->amount;
                    tem=tem->next;
                }
                if(len==-1)
                {
                    tem=block;
                    while(tem!=rhs.block)
                    {
                        if(tem==nullptr)
                        {
                            throw invalid_iterator();
                        }
                        len+=tem->amount;
                        tem=tem->next;
                    }
                    len+=rhs.pos-pos;
                    return len;
                }else
                {
                    len+=pos-rhs.pos;
                    return len;
                }
            }
            const_iterator& operator+=(const int &n) {
                int len=pos+n;
                while(len>=block->amount)
                {
                    len-=block->amount;
                    block=block->next;
                    if(block==nullptr)throw invalid_iterator();
                }
                pos=len;
                return *this;
            }
            const_iterator& operator-=(const int &n) {
                int len=pos-n;
                while(len<0)
                {
                    if(block->front==nullptr)throw invalid_iterator();
                    len+=block->front->amount;
                    block=block->front;
                }
                pos=len;
                return *this;
            }
            /**
             *  iter++
             */
            const_iterator operator++(int) {
                iterator ans(*this);
                (*this)+=1;
                return ans;
            }
            /**
             * TODO ++iter
             */
            const_iterator& operator++() {
                (*this)+=1;
                return *this;
            }
            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                iterator ans(*this);
                (*this)-=1;
                return ans;
            }
            /**
             * TODO --iter
             */
            const_iterator& operator--() {
                (*this)-=1;
                return *this;
            }
            /**
             * TODO *it
             * 		throw if iterator is invalid
             */
            T& operator*() const {
                if(block->storage[pos]==nullptr)throw invalid_iterator();
                return *(block->storage[pos]);
            }
            /**
             * TODO it->field
             * 		throw if iterator is invalid
             */
            T* operator->() const noexcept {
                return block->storage[pos];
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
                if(father==rhs.father&&block==rhs.block&&pos==rhs.pos)return true;
                else return false;
            }
            bool operator==(const const_iterator &rhs) const {
                if(father==rhs.father&&block==rhs.block&&pos==rhs.pos)return true;
                else return false;
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !((*this)==rhs);
            }
            bool operator!=(const const_iterator &rhs) const {
                return !((*this)==rhs);
            }
        };
        /**
         * Constructors
         */
        deque() {}
        deque(const deque &other) {
            copy_space(other);
        }
        /**
         * Deconstructor
         */
        ~deque() {
            delete_space();
        }
        /**
         * assignment operator
         */
        deque &operator=(const deque &other) {
            if(&other!=this)
            {
                delete_space();
                copy_space(other);
            }
            return *this;
        }
        /**
         * access specified element with bounds checking
         * throw index_out_of_bound if out of bound.
         */
        T & at(const size_t &pos) {
            auto iter=head;
            int sum=0;
            while(iter!=nullptr)
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
        const T & at(const size_t &pos) const {
            auto iter=head;
            int sum=0;
            while(iter!=nullptr)
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
        T & operator[](const size_t &pos) {
            auto iter=head;
            int sum=0;
            while(iter!=nullptr)
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
        const T & operator[](const size_t &pos) const {
            auto iter=head;
            int sum=0;
            while(iter!=nullptr)
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
        const T & front() const {
            if(head==nullptr)throw container_is_empty();
            return *(head->storage[0]);
        }
        /**
         * access the last element
         * throw container_is_empty when the container is empty.
         */
        const T & back() const {
            if(head==nullptr)throw container_is_empty();
            return *(tail->storage[tail->amount-1]);
        }
        /**
         * todo
         * returns an iterator to the beginning.
         */
        iterator begin() {
            return iterator(this,0,head);
        }
        const_iterator cbegin() const {
            return const_iterator(this,0,head);
        }
        /**
         * returns an iterator to the end.
         */
        iterator end() {
            return iterator(this,tail->amount,tail);
        }
        const_iterator cend() const {
            return const_iterator(this,tail->amount,tail);
        }
        /**
         * checks whether the container is empty.
         */
        bool empty() const {
            return head==nullptr;
        }
        /**
         * returns the number of elements
         */
        size_t size() const {
            size_t sum=0;
            auto it=head;
            while(it!=nullptr)
            {
                sum+=it->amount;
                it=it->next;
            }
            return sum;
        }
        /**
         * clears the contents
         */
        void clear() {
            delete_space();
        }
        /**
         * todo
         * inserts elements at the specified locat on in the container.
         * inserts value before pos
         * returns an iterator pointing to the inserted value
         *     throw if the iterator is invalid or it point to a wrong place.
         */
        iterator insert(iterator pos, const T &value) {
            if(pos.block->storage[pos.pos]==nullptr)throw invalid_iterator();
            if(pos.block->amount==Size_of_block)
            {
                node* tem=new node;
                for(int i=Size_of_block/2;i<Size_of_block;i++)
                {
                    tem->storage[i-Size_of_block/2]=pos.block->storage[i];
                    pos.block->storage[i]=nullptr;
                    pos.block->amount--;
                    tem->amount++;
                }
                node* tem_p=pos.block->next;
                tem->front=pos.block;
                tem->next=tem_p;
                pos.block->next=tem;
                tem_p->front=tem;
                if(pos.pos<Size_of_block/2)
                {
                    for(int i=pos.block->amount;i>pos.pos;i--)
                    {
                        pos.block->storage[i]=pos.block->storage[i-1];
                    }
                    pos.block->storage[pos.pos]=new T(value);
                    pos.block->amount++;
                    return iterator(this,pos.pos,pos.block);
                }else
                {
                    for(int i=tem->amount;i>pos.pos-Size_of_block/2;i--)
                    {
                        tem->storage[i]=tem->storage[i-1];
                    }
                    tem->storage[pos.pos-Size_of_block/2]=new T(value);
                    tem->amount++;
                    return iterator(this,pos.pos-Size_of_block/2,pos.block);
                }
            }else
            {
                for(int i=pos.block->amount;i>pos.pos;i--)
                {
                    pos.block->storage[i]=pos.block->storage[i-1];
                }
                pos.block->storage[pos.pos]=new T(value);
                pos.block->amount++;
                return iterator(this,pos.pos,pos.block);
            }
        }
        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos) {
            if(pos.block->storage[pos.pos]==nullptr)throw invalid_iterator();
            delete pos.block->storage[pos.pos];
            pos.block->amount--;
            if(pos.block->amount<Min_size&&pos.block->front!=nullptr)
            {
                if(pos.block->front->amount<Min_size)
                {
                    for(int i=0;i<pos.block->amount;i++)
                    {
                        pos.block->front->storage[pos.block->front->amount+i]=pos.block->storage[i];
                        pos.block->storage[i]=nullptr;
                    }
                    pos.block->front->amount=pos.block->front->amount+pos.block->amount;
                    auto tem=pos.block->front;
                    auto tem_p=pos.block->next;
                    delete pos.block;
                    tem->next=tem_p;
                    return iterator(this,pos.pos,pos.block);
                }else
                {
                    for(int i=pos.block->amount;i>0;i--)
                    {
                        pos.block->storage[i]=pos.block->storage[i-1];
                    }
                    pos.block->storage[0]=pos.block->front->storage[pos.block->front->amount-1];
                    pos.block->front->storage[pos.block->front->amount-1]=nullptr;
                    pos.block->front->amount--;
                    pos.block->amount++;
                    return iterator(this,pos.pos,pos.block);
                }
            }
            return iterator(this,pos.pos,pos.block);
            /*if(pos.block->amount==0)
            {
                delete pos.block;
                tail=nullptr;
                head=nullptr;
            }*/
        }
        /**
         * adds an element to the end
         */
        void push_back(const T &value) {
            if(tail->amount==Size_of_block)
            {
                node* tem=new node;
                for(int i=Size_of_block/2;i<Size_of_block;i++)
                {
                    tem->storage[i-Size_of_block/2]=tail->storage[i];
                    tail->storage[i]=nullptr;
                    tail->amount--;
                    tem->amount++;
                }
                tem->front=tail;
                tail->next=tem;
                tail=tem;
                tail->storage[tail->amount]=new T(value);
                tail->amount++;
            }else
            {
                tail->storage[tail->amount]=new T(value);
                tail->amount++;
            }
        }
        /**
         * removes the last element
         *     throw when the container is empty.
         */
        void pop_back() {
            if(head==nullptr)throw container_is_empty();
            delete tail->storage[tail->amount-1];
            tail->storage[tail->amount-1]=nullptr;
            tail->amount--;
            if(tail->amount<Min_size&&tail->front!=nullptr)
            {
                if(tail->front->amount<Min_size)
                {
                    for(int i=0;i<tail->amount;i++)
                    {
                        tail->front->storage[tail->front->amount+i]=tail->storage[i];
                        tail->storage[i]=nullptr;
                    }
                    tail->front->amount=tail->front->amount+tail->amount;
                    auto tem=tail->front;
                    delete tail;
                    tail=tem;
                    tail->next=nullptr;
                }else
                {
                    for(int i=tail->amount;i>0;i--)
                    {
                        tail->storage[i]=tail->storage[i-1];
                    }
                    tail->storage[0]=tail->front->storage[tail->front->amount-1];
                    tail->front->storage[tail->front->amount-1]=nullptr;
                    tail->front->amount--;
                    tail->amount++;
                }
            }
            if(tail->amount==0)
            {
                delete tail;
                tail=nullptr;
                head=nullptr;
            }
        }
        /**
         * inserts an element to the beginning.
         */
        void push_front(const T &value) {
            if(head->amount==Size_of_block)
            {
                node* tem=new node;
                tem->amount=1;
                for(int i=Size_of_block/2;i<Size_of_block;i++)
                {
                    tem->storage[i-Size_of_block/2+1]=head->storage[i];
                    head->storage[i]=nullptr;
                    tem->amount++;
                    head->amount--;
                }
                tem->storage[0]=new T(value);
                tem->next=head->next;
                tem->front=head;
                head->next=tem;
            }else
            {
                for(int i=head->amount;i>0;i--)
                {
                    head->storage[i]=head->storage[i-1];
                }
                head->amount++;
                head->storage[0]=new T(value);
            }
        }
        /**
         * removes the first element.
         *     throw when the container is empty.
         */
        void pop_front() {
            if(head==nullptr)throw container_is_empty();
            delete head->storage[0];
            for(int i=0;i<head->amount-1;i++)
            {
                head->storage[i]=head->storage[i+1];
            }
            head->storage[head->amount-1]=nullptr;
            head->amount++;
            if(head->amount==Min_size&&head->next!=nullptr)
            {
                if(head->next->amount==Min_size)
                {
                    for(int i=0;i<head->next->amount;i++)
                    {
                        head->storage[head->amount]=head->next->storage[i];
                        head->amount++;
                    }
                    auto tem=head->next;
                    head->next=head->next->next;
                    head->next->front=head;
                }else
                {
                    head->storage[head->amount]=head->next->storage[0];
                    for(int i=0;i<head->next->amount-1;i++)
                    {
                        head->next->storage[i]=head->next->storage[i+1];
                    }
                    head->next->storage[head->next->amount-1]=nullptr;
                    head->next->amount--;
                }
            }
            if(head->amount==0)
            {
                delete head;
                head=nullptr;
                tail=nullptr;
            }
        }
    };

}

#endif
