#ifndef  PRIMERY_KEYGEN_FILE
#define  PRIMERY_KEYGEN_FILE

#include <string.h>
#include <vector>
//#include <ostream>
#include <poseidon/precompiled.hpp>
#include <poseidon/time.hpp>

namespace PRIMERY_KEYGEN
{
   #define m_split  ","

	class GenIDS
	{
	public:
		GenIDS(){};
		virtual ~GenIDS(){};
	public:

		// make random uuid string
        static std::string GenId()
		{
		   //return (Poseidon::Uuid::random().to_string());
		   return "";
		}

		// make the uuid string
		static std::string GenId(const Poseidon::Uuid &key_1)
		{
			return  (boost::lexical_cast<std::string>(key_1));
		}

		static std::string GenDId(const Poseidon::Uuid &key_1)
		{
		   return (boost::lexical_cast<std::string>(key_1) + m_split);
		}

		static std::string GenDId(const int64_t &key_1)
		{
		  return (boost::lexical_cast<std::string>(key_1) + m_split);
		}

        static std::string GenDId(const uint64_t &key_1)
        {
          return (boost::lexical_cast<std::string>(key_1) + m_split);
        }

        // make int64 string
        static std::string GenId(const int64_t &key_1)
        {

            return  (boost::lexical_cast<std::string>(key_1));
        }
        // make uint64 string
        static std::string GenId(const uint64_t &key_1)
        {
            return  (boost::lexical_cast<std::string>(key_1));
        }
        static std::string GenId(const int64_t &key_1,const int64_t &key_2)
		{

			return  (GenId(key_1) + m_split + GenId(key_2));
			/*std::ostringstream oss;
			oss <<  key_1;
			oss <<  m_split;
			oss <<  key_2;
			return oss.str();*/
		}

        static std::string GenId(const int64_t &key_1,const int64_t &key_2,const int64_t &key_3)
		{

		   return  (GenId(key_1) + m_split + GenId(key_2) + m_split + GenId(key_3));
		}

		static std::string GenId(const int64_t &key_1,const int64_t &key_2,const int64_t &key_3,const int64_t &key_4)
		{

		   return  (GenId(key_1) + m_split + GenId(key_2) + m_split + GenId(key_3) + m_split + GenId(key_4));
		}

		// make continuos  uuid string
        static std::string GenId(const Poseidon::Uuid& key_1, const Poseidon::Uuid& key_2)
    	{
	    	return  (GenId(key_1) + m_split + GenId(key_2));
	    }

        static std::string GenId(const Poseidon::Uuid& key_1, const Poseidon::Uuid& key_2, const Poseidon::Uuid& key_3)
	    {
	    	 return  (GenId(key_1) + m_split + GenId(key_2) + m_split + GenId(key_3));
	    }


		// make uuid & int64 combination string
        static std::string GenId(const Poseidon::Uuid& key_1, const Poseidon::Uuid& key_2,const int64_t& key_3)
    	{
	    	 return  (GenId(key_1) + m_split + GenId(key_2) + m_split + GenId(key_3));
        }


	    static std::string GenId(const Poseidon::Uuid& key_1, const int64_t& key_2)
	    {
		   return  (GenId(key_1) + m_split + GenId(key_2));
	    }


		static std::string GenId(const Poseidon::Uuid& key_1, const int64_t& key_2, const int64_t& key_3)
		{
			 return  (GenId(key_1) + m_split + GenId(key_2) + m_split + GenId(key_3));
		}

		static std::string GenId(const Poseidon::Uuid& key_1, const int64_t& key_2,
					const int64_t& key_3,const int64_t& key_4)
		{
			 return  (GenId(key_1) + m_split + GenId(key_2)
						 + m_split + GenId(key_3) + m_split + GenId(key_4));
		}


		static std::string GenId(const Poseidon::Uuid& key_1, const int64_t& key_2,
					const int64_t& key_3,const int64_t& key_4,const int64_t& key_5)
		{
			return  (GenId(key_1) + m_split + GenId(key_2)
						+ m_split + GenId(key_3) + m_split + GenId(key_4)+ m_split + GenId(key_5));
		}


		static std::string GenId(const Poseidon::Uuid& key_1, const int64_t& key_2,
					const int64_t& key_3,const int64_t& key_4,const int64_t& key_5,
					const int64_t& key_6)
		{
			return  (GenId(key_1) + m_split + GenId(key_2)
						+ m_split + GenId(key_3) + m_split + GenId(key_4)
						+ m_split + GenId(key_5)+ m_split + GenId(key_6));
		}

		// make coordinate(x,y,dx,dy) string
        static std::string GenCID(std::int64_t coord_x,std::int64_t coord_y)
		{
		   std::int64_t x =(floor(double(coord_x/600)+ 1e-6) * 600);
		   std::int64_t y =(floor(double(coord_y/640)+ 1e-6) * 640);
		   std::int64_t x_offset =  coord_x - x;
		   std::int64_t y_offset =  coord_y - y;

			return (boost::lexical_cast<std::string>(x)+ m_split
						+ boost::lexical_cast<std::string>(y)+ m_split
						+ boost::lexical_cast<std::string>(x_offset) + m_split
						+ boost::lexical_cast<std::string>(y_offset));
		}

        static std::string GenCID_Prefix(std::int64_t coord_x,std::int64_t coord_y)
		{
		   std::int64_t x =(floor(double(coord_x/600)+ 1e-6) * 600);
		   std::int64_t y =(floor(double(coord_y/640)+ 1e-6) * 640);

			return (boost::lexical_cast<std::string>(x)+ m_split
						+ boost::lexical_cast<std::string>(y));
		}

		  // make coordinate(x,y,dx,dy) string
	    static std::string GenBID(std::int64_t coord_x,std::int64_t coord_y)
		{
		   std::int64_t x =(floor(double(coord_x/30)+ 1e-6) * 30);
		   std::int64_t y =(floor(double(coord_y/32)+ 1e-6) * 32);
		   std::int64_t x_offset =  coord_x - x;
		   std::int64_t y_offset =  coord_y - y;

			return (boost::lexical_cast<std::string>(x)+ m_split
						+ boost::lexical_cast<std::string>(y)+ m_split
						+ boost::lexical_cast<std::string>(x_offset) + m_split
						+ boost::lexical_cast<std::string>(y_offset));
		}

		static std::string GenBID_Prefix(std::int64_t coord_x,std::int64_t coord_y)
		{
		   std::int64_t x =(floor(double(coord_x/30)+ 1e-6) * 30);
		   std::int64_t y =(floor(double(coord_y/32)+ 1e-6) * 32);

			return (boost::lexical_cast<std::string>(x)+ m_split
						+ boost::lexical_cast<std::string>(y));
		}

	// make coordinate(x,y,dx,dy,uuid1) combination string
        static std::string GenCID(std::int64_t coord_x,std::int64_t coord_y,const Poseidon::Uuid& key_1)
		{
			return (GenCID(coord_x,coord_y) + m_split + GenId(key_1));
		}


	// make coordinate(x,y,dx,dy,uuid1,uuid2) combination string
        static std::string GenCID(std::int64_t coord_x,std::int64_t coord_y,const Poseidon::Uuid& key_1,const Poseidon::Uuid& key_2)
		{
			return (GenCID(coord_x,coord_y) + m_split + GenId(key_1) + m_split + GenId(key_2));
		}

        // make coordinate(x,y,dx,dy,uuid1,key2) combination string
        static std::string GenCID(std::int64_t coord_x,std::int64_t coord_y,const Poseidon::Uuid& key_1,const int64_t& key_2)
		{
			return (GenCID(coord_x,coord_y) + m_split + GenId(key_1) + m_split + GenId(key_2));
		}


        // make coordinate(x,y,dx,dy,uuid1,key2,key3) combination string
        static std::string GenCID(std::int64_t coord_x,std::int64_t coord_y
					,const Poseidon::Uuid& key_1,const int64_t& key_2
					,const int64_t& key_3)
		{
			return (GenCID(coord_x,coord_y) + m_split
						+ GenId(key_1) + m_split
						+ GenId(key_2) + m_split
						+ GenId(key_3));
		}

	// make coordinate(x,y,dx,dy,uuid1,uuid2,key3,key4) combination string
        static std::string GenCID(std::int64_t coord_x,std::int64_t coord_y
					,const Poseidon::Uuid& key_1,const Poseidon::Uuid& key_2,
					const int64_t& key_3,const int64_t& key_4)
		{
			return (GenCID(coord_x,coord_y) + m_split
						+ GenId(key_1) + m_split
						+ GenId(key_2) + m_split
						+ GenId(key_3) + m_split
						+ GenId(key_4));
		}
	// make coordinate(x,y,dx,dy,key1) combination string
        static std::string GenCID(std::int64_t coord_x,std::int64_t coord_y,const int64_t& key_1)
		{
			return (GenCID(coord_x,coord_y) + m_split + GenId(key_1));
		}

	// make coordinate(x,y,dx,dy,key1,key2) combination string
        static std::string GenCID(std::int64_t coord_x,std::int64_t coord_y,const int64_t& key_1,const int64_t& key_2)
		{
			return (GenCID(coord_x,coord_y) + m_split
						+ GenId(key_1)+ m_split
						+ GenId(key_2));
		}
    //format time
       static std::string  format_times(std::uint64_t times)
       {
         char* buffer = NULL;
         Poseidon::format_time(buffer, 64, times, false);
         std::string str_time(buffer);
         return str_time;
       }

    // make coordinate(uid.id.time_1) combination string
       static std::string GenID_DateTime(const Poseidon::Uuid& uid1,const int64_t& id1,const boost::uint64_t & key_1)
       {
         return (GenDId(uid1) + GenDId(id1) + format_times(key_1));
       }

    // make coordinate(x,y.dx,dy.time_1) combination string
		static std::string GenCID_DateTime(const int64_t& coord_x,const int64_t& coord_y,const boost::uint64_t & key_1)
		{
			return (GenCID(coord_x,coord_y) + m_split + format_times(key_1));
		}
	// make coordinate(id,x,y.dx,dy,time_1)combination string
       static  std::string GenCID_DateTime(const int64_t& id1,const int64_t& coord_x,const int64_t& coord_y,const boost::uint64_t & key_1)
	   {
           return (GenCID(coord_x,coord_y) + m_split + GenDId(id1) + format_times(key_1));
	   }
	   static std::string GenCID_DateTime(const uint64_t& id1,const int64_t& coord_x,const int64_t& coord_y,const boost::uint64_t & key_1)
	   {
	       return (GenCID(coord_x,coord_y) + m_split + GenDId(id1) + format_times(key_1));
	   }

	// make coordinate(uid,x,y.dx,dy,time_1)combination string
	   static std::string GenCID_DateTime(const Poseidon::Uuid& uid1,const int64_t& coord_x,const int64_t& coord_y,const boost::uint64_t & key_1)
	   {
	       return (GenCID(coord_x,coord_y) + m_split + GenDId(uid1) + format_times(key_1));
	   }

	// make coordinate(id,x,y.dx,dy,time_1)combination string
	   static std::string GenCID_DateTime(const Poseidon::Uuid& uid1,const int64_t& id1,const int64_t& coord_x,const int64_t& coord_y, const boost::uint64_t & key_1)
	   {
		   return (GenCID(coord_x,coord_y) + m_split + GenDId(uid1) + GenDId(id1) +format_times(key_1));
	   }
	   	// make coordinate(id,x,y.dx,dy,time_1)combination string
	   static std::string GenCID_DateTime(const Poseidon::Uuid& uid1,const uint64_t& id1,const int64_t& coord_x,const int64_t& coord_y,const boost::uint64_t& key_1)
	   {
		   return (GenCID(coord_x,coord_y) + m_split + GenDId(uid1) +GenDId(id1) + format_times(key_1));
	   }
 
	   // make coordinate(uid.id.string_1) combination string
       static std::string GenID(const Poseidon::Uuid& uid1,const int64_t& id1,const std::string & key_1)
       {
         return (GenDId(uid1) + GenDId(id1) + m_split + key_1);
       }
       // make coordinate(uid.id.string_1) combination string
       static std::string GenID(const Poseidon::Uuid& uid1,const std::string & key_1)
       {
         return (GenDId(uid1)  + key_1);
       }

       // make coordinate(uid.id.string_1) combination string
       static std::string GenID(const int64_t& id1,const std::string & key_1)
       {
         return (GenDId(id1) + m_split + key_1);
       }
	};
}
#endif
