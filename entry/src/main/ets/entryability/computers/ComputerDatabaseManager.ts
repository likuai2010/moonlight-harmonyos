import relationalStore from '@ohos.data.relationalStore'; // 导入模块
import common from '@ohos.app.ability.common'; // 导入模块
import { ComputerDetails, ComputerState } from './ComputerDetails';
import { NvHttp } from '../http/NvHttp';

export class ComputerDatabaseManager {
  store: relationalStore.RdbStore

  constructor() {
  }

  init(context: common.UIAbilityContext) {
    const STORE_CONFIG = {
      name: 'Computer.db', // 数据库文件名
      securityLevel: relationalStore.SecurityLevel.S1 // 数据库安全级别
    };
    relationalStore.getRdbStore(context, STORE_CONFIG, (err, store) => {
      this.store = store;
      this.initDb()
    })
  }

  initDb() {
    try {
      const SQL_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS Computers(UUID TEXT PRIMARY KEY, ComputerName TEXT NOT NULL, Addresses TEXT NOT NULL, MacAddress TEXT, ServerCert TEXT)"
      this.store.executeSql(SQL_CREATE_TABLE); // 创建数据表
    } catch (e) {
      console.error(e)
    }
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
      ServerCert: `${details.serverCert}`
    }
    if (old.rowCount > 0)
      this.store.update(data, predicates)
    else
      this.store.insert("Computers", data)
  }

  async getComputerByUUID(uuid: string): Promise<ComputerDetails> {
    const predicates = new relationalStore.RdbPredicates('Computers')
    predicates.equalTo('UUID', uuid);
    const cursor = await this.store.query(predicates);
    if (cursor.columnCount > 0) {
      cursor.goToNextRow();
      return this.getComputerFromCursor(cursor);
    } else {
      return null;
    }
  }

  async getAllComputers(): Promise<ComputerDetails[]> {
    const predicates = new relationalStore.RdbPredicates('Computers')
    const cursor = await this.store.query(predicates)
    const list = []
    cursor.goToNextRow()
    while (cursor.rowIndex >= 0 && cursor.rowIndex < cursor.rowCount) {
      list.push(this.getComputerFromCursor(cursor))
      cursor.goToNextRow()
    }
    return list;
  }

  getComputerFromCursor(c: relationalStore.ResultSet): ComputerDetails {
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
    details.serverCert = (c.getString(4) == "true")
    details.serverCert = true
    // This signifies we don't have dynamic state (like pair state)
    details.state = ComputerState.UNKNOWN;
    return details;
  }
}

const computerDatabaseManager = new ComputerDatabaseManager();

export default computerDatabaseManager