#ifndef _SL_INPUT_H_
#define _SL_INPUT_H_

#include <string>
#include "base.h"

namespace SL {
	typedef unsigned char ReadAccsFlag;
	const ReadAccsFlag RAF_COLUMN    = 0x01;
	const ReadAccsFlag RAF_ROW       = 0x02;
	const ReadAccsFlag RAF_SKIPNL    = 0x04;
	const ReadAccsFlag RAF_NLASS_NULL= 0x08;

	int read_accounts_data_from_file(const std::string path,
			Stor& to,
			const std::string fmt,
			const ReadAccsFlag flags);
};

#endif
