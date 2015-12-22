#ifndef EMPERY_CENTER_CASTLE_HPP_
#define EMPERY_CENTER_CASTLE_HPP_

#include "map_object.hpp"
#include <poseidon/cxx_util.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/function.hpp>
#include <vector>
#include "transaction_element_fwd.hpp"

namespace EmperyCenter {

namespace MySql {
	class Center_CastleBuildingBase;
	class Center_CastleResource;
	class Center_CastleTech;
}

class PlayerSession;

class Castle : public MapObject {
public:
	enum Mission {
		MIS_NONE            = 0,
		MIS_CONSTRUCT       = 1,
		MIS_UPGRADE         = 2,
		MIS_DESTRUCT        = 3,
//		MIS_PRODUCE         = 4,
	};

	struct BuildingBaseInfo {
		BuildingBaseId building_base_id;
		BuildingId building_id;
		unsigned building_level;
		Mission mission;
		boost::uint64_t mission_duration;
		boost::uint64_t mission_time_begin;
		boost::uint64_t mission_time_end;
	};

	struct TechInfo {
		TechId tech_id;
		unsigned tech_level;
		Mission mission;
		boost::uint64_t mission_duration;
		boost::uint64_t mission_time_begin;
		boost::uint64_t mission_time_end;
	};

	struct ResourceInfo {
		ResourceId resource_id;
		boost::uint64_t amount;
	};

private:
	boost::container::flat_map<BuildingBaseId,
		boost::shared_ptr<MySql::Center_CastleBuildingBase>> m_buildings;
	boost::container::flat_map<TechId,
		boost::shared_ptr<MySql::Center_CastleTech>> m_techs;
	boost::container::flat_map<ResourceId,
		boost::shared_ptr<MySql::Center_CastleResource>> m_resources;
	bool m_locked_by_transaction = false;

public:
	Castle(MapObjectUuid map_object_uuid,
		AccountUuid owner_uuid, MapObjectUuid parent_object_uuid, std::string name, Coord coord);
	Castle(boost::shared_ptr<MySql::Center_MapObject> obj,
		const std::vector<boost::shared_ptr<MySql::Center_MapObjectAttribute>> &attributes,
		const std::vector<boost::shared_ptr<MySql::Center_CastleBuildingBase>> &buildings,
		const std::vector<boost::shared_ptr<MySql::Center_CastleTech>> &techs,
		const std::vector<boost::shared_ptr<MySql::Center_CastleResource>> &resources);
	~Castle();

public:
	void pump_status() override;

	BuildingBaseInfo get_building_base(BuildingBaseId building_base_id) const;
	void get_all_building_bases(std::vector<BuildingBaseInfo> &ret) const;
	std::size_t count_buildings_by_id(BuildingId building_id) const;
	void get_buildings_by_id(std::vector<BuildingBaseInfo> &ret, BuildingId building_id) const;
	// 如果指定地基上有任务会抛出异常。
	void create_building_mission(BuildingBaseId building_base_id, Mission mission, BuildingId building_id = BuildingId());
	void cancel_building_mission(BuildingBaseId building_base_id);
	void speed_up_building_mission(BuildingBaseId building_base_id, boost::uint64_t delta_duration);

	void pump_building_status(BuildingBaseId building_base_id);
	unsigned get_building_queue_size() const;
	void synchronize_building_with_player(BuildingBaseId building_base_id, const boost::shared_ptr<PlayerSession> &session) const;

	// 各个建筑的独立接口。
	unsigned get_level() const; // 领主府
	boost::uint64_t get_max_resource_amount(ResourceId resource_id) const; // 仓库

	TechInfo get_tech(TechId tech_id) const;
	void get_all_techs(std::vector<TechInfo> &ret) const;
	// 同上。
	void create_tech_mission(TechId tech_id, Mission mission);
	void cancel_tech_mission(TechId tech_id);
	void speed_up_tech_mission(TechId tech_id, boost::uint64_t delta_duration);

	void pump_tech_status(TechId tech_id);
	unsigned get_tech_queue_size() const;
	void synchronize_tech_with_player(TechId tech_id, const boost::shared_ptr<PlayerSession> &session) const;

	ResourceInfo get_resource(ResourceId resource_id) const;
	void get_all_resources(std::vector<ResourceInfo> &ret) const;
	ResourceId commit_resource_transaction_nothrow(const std::vector<ResourceTransactionElement> &transaction,
		const boost::function<void ()> &callback = boost::function<void ()>());
	void commit_resource_transaction(const std::vector<ResourceTransactionElement> &transaction,
		const boost::function<void ()> &callback = boost::function<void ()>());

	void synchronize_with_player(const boost::shared_ptr<PlayerSession> &session) const;
};

inline void synchronize_castle_with_player(const boost::shared_ptr<const Castle> &castle,
	const boost::shared_ptr<PlayerSession> &session)
{
	castle->synchronize_with_player(session);
}
inline void synchronize_castle_with_player(const boost::shared_ptr<Castle> &castle,
	const boost::shared_ptr<PlayerSession> &session)
{
	castle->synchronize_with_player(session);
}

}

#endif
