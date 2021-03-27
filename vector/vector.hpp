#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "../../MyVector/exceptions.hpp"

#include <climits>
#include <cstddef>
#include <iostream>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
    template<typename T>
    class vector {
    private:
        T** _Begin=nullptr;
        T** _storeEnd=nullptr;
        T** _dataEnd=nullptr;
        void _build(int size=8)
        {
            T** tem=new T*[size];
            /*if(_Begin!=nullptr)
            {
                //memcpy(tem,_Begin,(_storeEnd-_Begin+1)*sizeof(T));
                for(auto it=_Begin;it!=_storeEnd+1;it++)
                {
                    tem[it-_Begin]=*it;
                    *it=nullptr;
                }
            }
            _deleteStorage();
            if(_Begin!=nullptr)_dataEnd=tem+(_storeEnd-_Begin);
            else _dataEnd=tem-1;
            _storeEnd=tem+size-1;
            _Begin=tem;*/

            if(_Begin!=nullptr)
            {
                for(auto it=_Begin;it!=_storeEnd+1;it++)
                {
                    tem[it-_Begin]=*it;
                    *it=nullptr;
                }
                int _size_=_storeEnd-_Begin+1;
                _deleteStorage();
                _Begin=tem;
                _dataEnd=tem+_size_-1;
                _storeEnd=tem+size-1;
            }else
            {
                _Begin=tem;
                _storeEnd=tem+size-1;
                _dataEnd=tem-1;
            }
        }
        void _rebuildMore()
        {
            size_t NewSize=2*(_dataEnd-_Begin+1);
            _build(NewSize);
        }
        void _deleteStorage()
        {
            auto it=_Begin;
            if(it==nullptr)return;
            else
            {
                while(_dataEnd-it>=0)
                {
                    if(*it!=nullptr)delete *it;
                    it++;
                }
                delete []_Begin;
                _Begin=nullptr;
            }
        }
    public:
        /**
         * a type for actions of the elements of a vector, and you should write
         *   a class named const_iterator with same interfaces.
         */
        /**
         * you can see RandomAccessIterator at CppReference for help.
         */
        class const_iterator;
        class iterator {
        private:
            /**
             * add data members
             * just add whatever you want.
             */
            int _header=1;//1向后，-1向前
            vector* _father=nullptr;
            T** _where=nullptr;
        public:
            /**
             *
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            iterator(vector* father=nullptr,T** where=nullptr,int header=1):_header(header),_father(father),_where(where) {	}
            iterator(const iterator &o):_header(o._header),_father(o._father),_where(o._where){}
            iterator operator+(const int &n) const {
                int __pos=_where-_father->_Begin+n*_header;
                if(__pos<-1+(1+_header)/2 || __pos>(_father->_dataEnd-_father->_Begin)+(1+_header)/2)throw invalid_iterator();
                else{
                    iterator ans(*this);
                    ans._where+=n*_header;
                    return ans;
                }
            }
            iterator operator-(const int &n) const {
                return ((*this)+(-n));
            }
            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                if((this->_father)!=rhs._father)throw invalid_iterator();
                else return _where-rhs._where;
            }
            iterator& operator+=(const int &n) {
                (*this)=(*this)+n;
                return *this;
            }
            iterator& operator-=(const int &n) {
                return ((*this)+=(-n));
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
             *  ++iter
             */
            iterator& operator++() {
                return((*this)+=1);
            }
            /**
             *  iter--
             */
            iterator operator--(int) {
                iterator ans(*this);
                (*this)+=-1;
                return ans;
            }
            /**
             *  --iter
             */
            iterator& operator--() {
                return((*this)+=-1);
            }
            /**
             *
             */
            T& operator*() const{
                if(_father==nullptr || _where>_father->_dataEnd || _where < _father->_Begin)throw invalid_iterator();
                else return **_where;
            }
            /**
             *
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                if(_header==rhs._header && _father==rhs._father && _where==rhs._where)return true;
                else return false;
            }
            bool operator==(const const_iterator &rhs) const {
                if(_header==rhs._header && _father==rhs._father && _where==rhs._where)return true;
                else return false;
            }
            /**'
             *
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
         *
         * has same function as iterator, just for a const object.
         */
        class const_iterator {
        private:
            /**
             * add data members
             * just add whatever you want.
             */
            const int _header=1;//1向后，-1向前
            const vector* _father;
            T** _where=nullptr;
        public:
            /**
             *
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            const_iterator(const vector* father=nullptr,T** where=nullptr,int header=1):_header(header),_father(father),_where(where) {	}
            const_iterator(const const_iterator &o):_header(o._header),_father(o._father),_where(o._where){}
            const_iterator operator+(const int &n) const {
                int __pos=_where-_father->_dataBegin+n*_header;
                if(__pos<-1+(1+_header)/2 || __pos>(_father->_dataEnd-_father->_dataBegin)+(1+_header)/2)throw invalid_iterator();
                else
                {
                    const_iterator ans(*this);
                    ans._where+=n*_header;
                    return ans;
                }
            }
            const_iterator operator-(const int &n) const {
                return ((*this)+(-n));
            }
            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const const_iterator &rhs) const {
                if((*this)!=rhs)throw invalid_iterator();
                else return _where-rhs._where;
            }
            const_iterator& operator+=(const int &n) {
                int __pos=_where-_father->_Begin+n*_header;
                if(__pos<-1+(1+_header)/2 || __pos>(_father->_dataEnd-_father->_Begin)+(1+_header)/2)throw invalid_iterator();
                _where+=n*_header;
                return *this;
            }
            const_iterator& operator-=(const int &n) {
                int __pos=_where-_father->_dataBegin-n*_header;
                if(__pos<-1+(1+_header)/2 || __pos>(_father->_dataEnd-_father->_dataBegin)+(1+_header)/2)throw invalid_iterator();
                _where-=n*_header;
                return *this;
            }
            /**
             *  iter++
             */
            const_iterator operator++(int) {
                const_iterator ans(*this);
                (*this)+=1;
                return ans;
            }
            /**
             *  ++iter
             */
            const_iterator& operator++() {
                return((*this)+=1);
            }
            /**
             *  iter--
             */
            const_iterator operator--(int) {
                const_iterator ans(*this);
                (*this)+=-1;
                return ans;
            }
            /**
             *  --iter
             */
            const_iterator& operator--() {
                return((*this)+=-1);
            }
            /**
             *
             */
            const T& operator*() const{
                if(_father==nullptr || _where>_father->_dataEnd || _where < _father->_Begin)throw invalid_iterator();
                else return **_where;
            }
            /**
             *
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                if(_header==rhs._header && _father==rhs._father && _where==rhs._where)return true;
                else return false;
            }
            bool operator==(const const_iterator &rhs) const {
                if(_header==rhs._header && _father==rhs._father && _where==rhs._where)return true;
                else return false;
            }
            /**'
             *
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
         * Constructs
         * AtLeast two: default constructor, copy constructor
         */
        vector() {}
        vector(const vector &other) {
            int n=other._storeEnd-other._Begin;
            _build(n+1);
            _dataEnd=(other._dataEnd-other._Begin)+_Begin;
            //memcpy(_Begin,other._Begin,(n+1)*sizeof(T));
            for(int i=0;i<other._dataEnd-other._Begin+1;i++)
            {
                (_Begin[i])=new T(*(other._Begin[i]));
            }
        }
        /**
         * Destructor
         */
        ~vector() {
            _deleteStorage();
        }
        /**
         * Assignment operator
         */
        vector &operator=(const vector &other) {
            if(&other==this)return *this;
            else
            {
                _deleteStorage();
                int n=other._storeEnd-other._Begin;
                _build(n+1);
                _dataEnd=(other._dataEnd-other._Begin)+_Begin;
                //memcpy(_Begin,other._Begin,(n+1)*sizeof(T));
                for(int i=0;i<other._dataEnd-other._Begin+1;i++)
                {
                    (_Begin[i])=new T(*(other._Begin[i]));
                }
            }
            return *this;
        }
        /**
         *
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         */
        T & at(const size_t &pos) {
            if(pos<0 || pos>_dataEnd-_Begin)throw index_out_of_bound();
            else return **(_Begin+pos);
        }
        const T & at(const size_t &pos) const {
            if(pos<0 || pos>_dataEnd-_Begin)throw index_out_of_bound();
            else return **(_Begin+pos);
        }
        /**
         *
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */
        T & operator[](const size_t &pos) {
            if(pos<0 || pos>_dataEnd-_Begin)throw index_out_of_bound();
            else return **(_Begin+pos);
        }
        const T & operator[](const size_t &pos) const {
            if(pos<0 || pos>_dataEnd-_Begin)throw index_out_of_bound();
            else return **(_Begin+pos);
        }
        /**
         *
         * access the first element.
         * throw container_is_empty if size == 0
         */
        const T & front() const {
            if(_dataEnd<_Begin)throw container_is_empty();
            else return **_Begin;
        }
        /**
         *
         * access the last element.
         * throw container_is_empty if size == 0
         */
        const T & back() const {
            if(_dataEnd<_Begin)throw container_is_empty();
            else return **_dataEnd;
        }
        /**
         *
         * returns an iterator to the beginning.
         */
        iterator begin() {
            iterator ans(this,_Begin,1);
            return ans;
        }
        const_iterator rbegin() const {
            const_iterator ans(this,_dataEnd,-1);
            return ans;
        }
        const_iterator cbegin() const {
            const_iterator ans(this,_Begin,1);
            return ans;
        }
        /**
         *
         * returns an iterator to the end.
         */
        iterator end() {
            iterator ans(this,_dataEnd+1,1);
            return ans;
        }
        const_iterator rend() const {
            const_iterator ans(this,_Begin-1,-1);
            return ans;
        }
        const_iterator cend() const {
            const_iterator ans(this,_dataEnd+1,1);
            return ans;
        }
        /**
         * checks whether the container is empty
         */
        bool empty() const {
            if(_Begin==nullptr)return true;
            else if(_Begin==_dataEnd)return false;
        }
        /**
         * returns the number of elements
         */
        size_t size() const {
            if(_Begin==nullptr)return 0;
            else return _dataEnd-_Begin+1;
        }
        /**
         *
         * clears the contents
         */
        void clear() {
            for(auto it=_Begin;it!=_dataEnd+1;it++)
            {
                delete *it;
            }
            _dataEnd=_Begin-1;
        }
        /**
         *
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         */
        iterator insert(iterator pos, const T &value) {
            if(_Begin==nullptr)
            {
                _build();
            }
            int p=pos-this->begin();
            if(_dataEnd==_storeEnd)_rebuildMore();
            int base=0;
            for(int i=_dataEnd-_Begin;i>=p;i--)
            {
                _Begin[base+i+1]=_Begin[base+i];
            }
            _dataEnd++;
            _Begin[base+p]=new T(value);
            iterator ans(this,_Begin+base+p,1);
            return ans;
        }
        /**
         *
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
         */
        iterator insert(const size_t &ind, const T &value) {
            if(_Begin==nullptr)
            {
                _build();
            }
            if(ind>_dataEnd-_Begin+1)throw index_out_of_bound();
            int p=ind;
            if(_dataEnd==_storeEnd)_rebuildMore();
            int base=0;
            for(int i=_dataEnd-_Begin;i>=p;i--)
            {
                _Begin[base+i+1]=_Begin[base+i];
            }
            _dataEnd++;
            _Begin[base+p]=new T(value);
            iterator ans(this,_Begin+base+p,1);
            return ans;
        }
        /**
         *
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */
        iterator erase(iterator pos) {
            int p=(pos-this->begin());
            T** _p=_Begin+p;
            delete *_p;
            for(;_p!=_dataEnd;_p++)
            {
                *_p=*(_p+1);
                //memcpy(_p,_p+1,sizeof(T));
            }
            _dataEnd--;
            _p=_Begin+p;
            if(_p>_dataEnd)return this->end();
            else
            {
                iterator ans(this,_p,1);
                return ans;
            }
        }
        /**
         *
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= size
         */
        iterator erase(const size_t &ind) {
            if(ind>=_dataEnd-_Begin+1)throw index_out_of_bound();
            int p=(ind);
            T** _p=_Begin+p;
            delete *_p;
            for(;_p!=_dataEnd;_p++)
            {
                *_p=*(_p+1);
                //memcpy(_p,_p+1,sizeof(T));
            }
            _dataEnd--;
            _p=_Begin+p;
            if(_p>_dataEnd)return this->end();
            else
            {
                iterator ans(this,_p,1);
                return ans;
            }
        }
        /**
         *
         * adds an element to the end.
         */
        void push_back(const T &value) {
            if(_Begin==nullptr)
            {
                _build();
            }
            if(_dataEnd==_storeEnd)_rebuildMore();
            _dataEnd++;
            *_dataEnd=new T(value);
        }
        /**
         *
         * remove the last element from the end.
         * throw container_is_empty if size() == 0
         */
        void pop_back() {
            if(_dataEnd<_Begin)throw container_is_empty();
            delete *_dataEnd;
            _dataEnd--;
        }
    };


}

#endif
