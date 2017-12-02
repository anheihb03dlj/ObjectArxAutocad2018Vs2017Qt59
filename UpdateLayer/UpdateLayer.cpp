﻿#include <map>
#include <optional>
#include "UpdateLayer.hpp"

namespace sstd {

	extern void loadUpdateLayer() { UpdateLayer::load(); }

	UpdateLayer::UpdateLayer() {
	}

	void UpdateLayer::load(){
		arx_add_main_command<UpdateLayer>();
	}

	namespace {
#define simple_code_args const std::wstring_view & argNM , AcDbLayerTable * argLT, AcDbLinetypeTable * argLTT , AcDbLayerTableRecord * argLTR
		using ApplyLayerType = void(*)(simple_code_args);
		using ApplyMaps = std::map<std::wstring_view, std::pair<ApplyLayerType, bool> >;
		inline ApplyMaps _p_createFunctions() {
			ApplyMaps varAns;
			/********************************************************/
			varAns.emplace(LR"(0)"sv, ApplyMaps::value_type::second_type{ [](simple_code_args) {
				sstd::ArxClosePointer<AcDbLayerTableRecord> varLocalLTR;
				if (argLTR == nullptr) {
					varLocalLTR = new AcDbLayerTableRecord;
					argLTR = varLocalLTR;
					argLTR->setName(argNM.data());
					argLT->add(argLTR);
				}

				/*初始化颜色*****************************************************/
				AcCmColor varLColor;
				varLColor.setColorIndex(211);
				/*初始化线宽*****************************************************/
				AcDb::LineWeight varLWeight = AcDb::kLnWt030;
				/*设置透明度*****************************************************/
				AcCmTransparency varLTP;
				varLTP.setAlphaPercent(0);
				/*初始化线型*****************************************************/
				std::optional<AcDbObjectId> varLTypeID;
				{
					AcDbObjectId varLTypeIDTmp;
					if (Acad::eOk == argLTT->getAt(LR"(Continuous)", varLTypeIDTmp)) {
						*varLTypeID = varLTypeIDTmp;
					}
				}
				/******************************************************/
				argLTR->setIsOff(false)/*打开/关闭*/;
				argLTR->setIsFrozen(false)/*冻结*/;
				argLTR->setIsLocked(false)/*锁定*/;
				argLTR->setColor(varLColor)/*颜色*/;
				argLTR->setIsPlottable(true)/*打印*/;
				argLTR->setDescription(LR"(图层：0)")/*注释*/;
				if (varLTypeID) {
					argLTR->setLinetypeObjectId(*varLTypeID)/*设置线型*/;
				}
				argLTR->setLineWeight(varLWeight)/*线宽*/;
				argLTR->setTransparency(varLTP)/*透明度*/;

			} ,false});
			/********************************************************/
			return std::move(varAns);
		}
		
		inline void _p_update_layer( AcDbDatabase * argDB ) {
			if (nullptr == argDB) { return; }
			
			/*获得线型表*/
			sstd::ArxClosePointer< AcDbLinetypeTable > varLinetypeTable;
			if ( Acad::eOk != argDB->getLinetypeTable(varLinetypeTable) ) {
				acutPrintf(LR"(获得线型失败
)");
			}

			/*获得图层表*/
			sstd::ArxClosePointer< AcDbLayerTable > varLayerTable;
			if ( Acad::eOk != argDB->getLayerTable(varLayerTable) ) {
				acutPrintf(LR"(获得线型失败
)");
			}

			/**/
			auto varFunctions = _p_createFunctions();
			const auto varFNoPos = varFunctions.end();

			/*处理已经存在图层*/
			{
				std::unique_ptr<AcDbLayerTableIterator> varIt;
				{
					AcDbLayerTableIterator * varItTmp = nullptr;
					if (Acad::eOk != varLayerTable->newIterator(varItTmp)) {
						return ;
					}
					varIt.reset(varItTmp);
				}

				for (varIt->start(); !varIt->done(); varIt->step()) {
					sstd::ArxClosePointer< AcDbLayerTableRecord > varLTR;
					if (Acad::eOk == varIt->getRecord(varLTR, AcDb::kForWrite )) {
						wchar_t * varName = nullptr;
						if (Acad::eOk == varLTR->getName(varName)) {
							auto varCPos = varFunctions.find( varName );
							if (varCPos == varFNoPos) { continue; }
							varCPos->second.second = true;
							varCPos->second.first(varCPos->first,
								varLayerTable,
								varLinetypeTable,
								varLTR
							);
						}
					}
				}
			}

			/*创建新图层*/
			for (auto & varI: varFunctions) {
				if (varI.second.second==false) {
					varI.second.second = true;
					varI.second.first(varI.first, 
						varLayerTable, 
						varLinetypeTable,
						nullptr);
				}
			}

		}
	}/*namespace*/

	void UpdateLayer::main(){
		_p_update_layer(acdbHostApplicationServices()->workingDatabase());
	}

}/*namespace sstd*/


