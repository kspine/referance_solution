#ifndef EMPERY_CENTER_TASK_BOX_HPP_
#define EMPERY_CENTER_TASK_BOX_HPP_

#include <poseidon/cxx_util.hpp>
#include <poseidon/virtual_shared_from_this.hpp>
#include <cstddef>
#include <vector>
#include <boost/container/flat_map.hpp>
#include <boost/shared_ptr.hpp>
#include "id_types.hpp"

namespace EmperyCenter {
	namespace MongoDb {
		class Center_Task;
	}

	class Castle;
	class PlayerSession;

	class TaskBox : NONCOPYABLE, public virtual Poseidon::VirtualSharedFromThis {
	public:
		enum Category {
			CAT_NULL = 0,
			CAT_PRIMARY = 1,
			CAT_DAILY = 2,
			CAT_BRANCH = 3,
			CAT_LEGION_PACKAGE = 4,
			CAT_LEGION = 5,
		};

		enum CastleCategory {
			TCC_PRIMARY = 1,
			TCC_ALL = 2,
			TCC_NON_PRIMARY = 3,
		};

		using Progress = boost::container::flat_map<std::uint64_t, std::uint64_t>;

		struct TaskInfo {
			TaskId task_id;
			Category category;
			std::uint64_t created_time;
			std::uint64_t expiry_time;
			boost::shared_ptr<const Progress> progress;
			bool rewarded;
		};

	private:
		const AccountUuid m_account_uuid;

		boost::shared_ptr<MongoDb::Center_Task> m_stamps;

		boost::container::flat_map<TaskId,
			std::pair<boost::shared_ptr<MongoDb::Center_Task>, boost::shared_ptr<Progress>>> m_tasks;

	public:
		TaskBox(AccountUuid account_uuid,
			const std::vector<boost::shared_ptr<MongoDb::Center_Task>> &tasks);
		~TaskBox();

	public:
		virtual void pump_status();

		AccountUuid get_account_uuid() const {
			return m_account_uuid;
		}

		void check_primary_tasks();
		void check_daily_tasks();
		void check_daily_tasks_init();
		void check_daily_tasks_next(TaskId task_id);
		/*************************************************************************************************/
		void reset_legion_package_tasks();
		void check_legion_package_tasks();

		void update_reward_status(TaskId task_id);
		bool check_reward_status(TaskId task_id);

		void check_task_dungeon_clearance(std::uint64_t key_dungeon_id,std::uint64_t finish_count);
		void access_task_dungeon_clearance();
        void recheck_join_legion_tasks(AccountUuid account_uuid,TaskId task_id);


		//建筑升级刷新任务
		//void check_caster_legion_package_task();
		//void sync_legion_package_finished_task();
		/*************************************************************************************************/

		TaskInfo get(TaskId task_id) const;
		void get_all(std::vector<TaskInfo> &ret) const;

		void insert(TaskInfo info);
		void update(TaskInfo info, bool throws_if_not_exists = true);
		bool remove(TaskId task_id) noexcept;

		bool has_been_accomplished(TaskId task_id) const;
		void check(Category category,TaskTypeId type, std::uint64_t key, std::uint64_t count,
			CastleCategory castle_category, std::int64_t param1, std::int64_t param2);
		void check(Category category,TaskTypeId type, std::uint64_t key, std::uint64_t count,
			const boost::shared_ptr<Castle> &castle, std::int64_t param1, std::int64_t param2);

		void synchronize_with_player(const boost::shared_ptr<PlayerSession> &session) const;
	};
}

#endif