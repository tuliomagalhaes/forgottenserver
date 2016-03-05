function onUpdateDatabase()
	print("> Updating database to version 19 (update on depot chests)")
	db.query("UPDATE `player_depotitems` SET `pid` = 16 WHERE `pid` = 0")
	local resultId = db.storeQuery("SELECT `sid`, `pid` FROM `player_depotitems`")
	if resultId then
		repeat
			db.query("UPDATE `player_depotitems` SET `pid` = " .. result.getNumber(resultId, "pid") - 1 .. " WHERE `pid` < 16 AND `sid` = " .. result.getNumber(resultId, "sid"))
		until not result.next(resultId)
		result.free(resultId)
	end
	db.query("UPDATE `player_depotitems` SET `pid` = 16 WHERE `pid` > 16")
	return true
end
