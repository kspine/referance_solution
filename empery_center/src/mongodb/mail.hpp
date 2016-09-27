#ifndef EMPERY_CENTER_MONGODB_MAIL_HPP_
#define EMPERY_CENTER_MONGODB_MAIL_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_Mail
#define MONGODB_OBJECT_PRIMARY_KEY mail_uuid account_uuid
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (mail_uuid)	\
	FIELD_UUID              (account_uuid)	\
	FIELD_DATETIME          (expiry_time)	\
	FIELD_BOOLEAN           (system)	\
	FIELD_BOOLEAN           (read)	\
	FIELD_BOOLEAN           (attachments_fetched)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_MailData
#define MONGODB_OBJECT_PRIMARY_KEY mail_uuid language_id
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (mail_uuid)	\
	FIELD_SIGNED            (language_id)	\
	FIELD_DATETIME          (created_time)	\
	FIELD_SIGNED            (type)	\
	FIELD_UUID              (from_account_uuid)	\
	FIELD_STRING            (subject)	\
	FIELD_STRING            (segments)	\
	FIELD_STRING            (attachments)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
