#ifndef EMPERY_CENTER_LEGION_TASK_CONTRIBUTION_BOX_HPP_
#define EMPERY_CENTER_LEGION_TASK_CONTRIBUTION_BOX_HPP_

#include <poseidon/cxx_util.hpp>
#include <poseidon/virtual_shared_from_this.hpp>
#include <cstddef>
#include <vector>
#include <boost/container/flat_map.hpp>
#include <boost/shared_ptr.hpp>
#include "id_types.hpp"

namespace EmperyCenter {
	namespace MongoDb {
		class Center_LegionTaskContribution;
	}

	class PlayerSession;

	class LegionTaskContributionBox : NONCOPYABLE, public virtual Poseidon::VirtualSharedFromThis {
	public:

		struct TaskContributionInfo {
			AccountUuid   account_uuid;
			std::uint64_t day_contribution;
			std::uint64_t week_contribution;
			std::uint64_t total_contribution;
			std::uint64_t created_time;
		};

	private:
		const LegionUuid m_legion_uuid;

		boost::shared_ptr<MongoDb::Center_LegionTaskContribution> m_stamps;

		boost::container::flat_map<AccountUuid,
			boost::shared_ptr<MongoDb::Center_LegionTaskContribution>> m_contributions;

	public:
		LegionTaskContributionBox(AccountUuid account_uuid,
			const std::vector<boost::shared_ptr<MongoDb::Center_LegionTaskContribution>> &contributions);
		~LegionTaskContributionBox();

	public:
		LegionUuid get_legion_uuid() const {
			return m_legion_uuid;
		}

		TaskContributionInfo get(TaskId task_id) const;
		void get_all(std::vector<TaskContributionInfo> &ret) const;

		void insert(TaskContributionInfo info);
		void update(TaskContributionInfo info, bool throws_if_not_exists = true);
		void reset() noexcept;
	};
}

#endif