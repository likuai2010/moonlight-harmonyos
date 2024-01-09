import relationalStore from '@ohos.data.relationalStore'; // 导入模块
import Context from '@ohos.app.ability.Ability'; // 导入模块
im
import { ComputerDetails, ComputerState } from './ComputerDetails';
import { NvHttp } from '../http/NvHttp';

export class ComputerDatabaseManager {
  store: relationalStore.RdbStore

  constructor() {

  }

  init(context: BaseContext) {
    const STORE_CONFIG = {
      name: 'Computer.db', // 数据库文件名
      securityLevel: relationalStore.SecurityLevel.S1 // 数据库安全级别
    };
    relationalStore.getRdbStore(context, STORE_CONFIG, (err, store) => {
      this.store = store;
    })

  }

  initDb(c: Context) {
    const SQL_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS Computers(UUID TEXT PRIMARY KEY, ComputerName TEXT NOT NULL, Addresses TEXT NOT NULL, MacAddress TEXT, ServerCert TEXT)"
    this.store.executeSql(SQL_CREATE_TABLE); // 创建数据表

  }

  deleteComputer(details: ComputerDetails) {
    const predicates = new relationalStore.RdbPredicates('Computers')
    predicates.equalTo('UUID', details.uuid);
    this.store.delete(predicates);
  }

  async addOrUpdateComputer(details: ComputerDetails) {
    const predicates = new relationalStore.RdbPredicates('Computers')
    predicates.equalTo('UUID', details.uuid);
    const old = await this.store.query(predicates)
    const data = {
      UUID: details.uuid,
      ComputerName: details.name,
      Addresses: JSON.stringify({
        localAddress: details.localAddress,
        remoteAddress: details.remoteAddress,
        manualAddress: details.manualAddress,
        ipv6Address: details.ipv6Address,
      }),
      MacAddress: details.macAddress,
      ServerCert: null
    }
    if (old.rowCount > 0)
      this.store.update(data, predicates)
    else
      this.store.insert("Computers", data)
  }

  async getAllComputers() {
    const predicates = new relationalStore.RdbPredicates('Computers')
    const cursor = await this.store.query(predicates)
    const list = []
    while (cursor.rowIndex < cursor.rowCount) {
      list.push(this.getComputerFromCursor(cursor))
      cursor.goToNextRow()
    }
  }

  getComputerFromCursor(c: relationalStore.ResultSet) {
    const details = new ComputerDetails();
    details.uuid = c.getString(0);
    details.name = c.getString(1);
    try {
      const addresses = JSON.parse(c.getString(2));
      details.localAddress = addresses.localAddress;
      details.remoteAddress = addresses.remoteAddress;
      details.manualAddress = addresses.manualAddress;
      details.ipv6Address = addresses.ipv6Address;
    } catch (e) {
    }
    // External port is persisted in the remote address field
    if (details.remoteAddress != null) {
      details.externalPort = details.remoteAddress.port;
    }
    else {
      details.externalPort = NvHttp.DEFAULT_HTTP_PORT;
    }
    details.macAddress = c.getString(3);
    // This signifies we don't have dynamic state (like pair state)
    details.state = ComputerState.UNKNOWN;
    return details;
  }
}