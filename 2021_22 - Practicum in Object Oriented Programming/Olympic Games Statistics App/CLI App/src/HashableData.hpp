#ifndef HASHABLE_DATA_HPP
#define HASHABLE_DATA_HPP


namespace sale {


template<typename TKey_, typename KeyHashStruct_, typename KeyEqStruct_>
class HashableData
{

 public:

	using TKey = TKey_;
	using KeyHashStruct = KeyHashStruct_;
	using KeyEqStruct = KeyEqStruct_;

	virtual ~HashableData() = 0;
	
	virtual TKey getKey() const = 0;

};


template<typename TKey_, typename KeyHashStruct_, typename KeyEqStruct_>
HashableData<TKey_, KeyHashStruct_, KeyEqStruct_>::~HashableData() {

}


}  // end of sale namespace


#endif
