﻿#ifndef UPDATEMLEADERSTYLE_HPP
#define UPDATEMLEADERSTYLE_HPP

namespace sstd {

	class UpdateMLeaderStyle {
	public:
		UpdateMLeaderStyle();

		static void load();
		static void main();
		static void main_do_not_change_text_height();
		DEFINE_ARX_NAME(LR"(_updatemleaderstyle)")
	};

}/*namespace sstd*/

#endif // UPDATEMLEADERSTYLE_HPP
